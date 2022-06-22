#!/usr/bin/env python

import time, threading, queue, argparse
import numpy as np
import pvaccess as pva
import os, os.path

__version__ = pva.__version__

class AdSimServer:

    DELAY_CORRECTION = 0.0001
    NOTIFICATION_DELAY = 0.1
    PVA_TYPE_KEY_MAP = {
        np.dtype('uint8')   : 'ubyteValue',
        np.dtype('int8')    : 'byteValue',
        np.dtype('uint16')  : 'ushortValue',
        np.dtype('int16')   : 'shortValue',
        np.dtype('uint32')  : 'uintValue',
        np.dtype('int32')   : 'intValue',
        np.dtype('uint64')  : 'ulongValue',
        np.dtype('int64')   : 'longValue',
        np.dtype('float32') : 'floatValue',
        np.dtype('float64') : 'doubleValue'
    }

    def __init__(self, input_directory, input_file, mmap_mode, frame_rate, nf, nx, ny, datatype, minimum, maximum, runtime, channel_name, notify_pv, notify_pv_value, start_delay, report_frequency):
        self.arraySize = None
        self.delta_t = 0
        if frame_rate > 0:
            self.delta_t = 1.0/frame_rate
        self.runtime = runtime
        self.report_frequency = report_frequency 

        input_files = []
        if input_directory is not None:
            input_files = [os.path.join(input_directory, f) for f in os.listdir(input_directory) if os.path.isfile(os.path.join(input_directory, f))]
        if input_file is not None:
            input_files.append(input_file)
        self.frames = None
        for f in input_files:
            try:
                if mmap_mode:
                    new_frames = np.load(f, mmap_mode='r')
                else:
                    new_frames = np.load(f)
                if self.frames is None:
                    self.frames = new_frames
                else:
                    self.frames = np.append(self.frames, new_frames, axis=0)
                print('Loaded input file {}'.format(f))
            except Exception as ex:
                print('Cannot load input file {}, skipping it: {}'.format(f, str(ex)))
        if self.frames is None:
            print('Generating random frames')
            # Example frame:
            # frame = np.array([[0,0,0,0,0,0,0,0,0,0],
            #                  [0,0,0,0,1,1,0,0,0,0],
            #                  [0,0,0,1,2,3,2,0,0,0],
            #                  [0,0,0,1,2,3,2,0,0,0],
            #                  [0,0,0,1,2,3,2,0,0,0],
            #                  [0,0,0,0,0,0,0,0,0,0]], dtype=np.uint16)
            dt = np.dtype(datatype)
            if datatype != 'float32' and datatype != 'float64':
                dtinfo = np.iinfo(dt)
                mn = dtinfo.min
                if minimum is not None:
                    mn = int(max(dtinfo.min, minimum))
                mx = dtinfo.max
                if maximum is not None:
                    mx = int(min(dtinfo.max, maximum))
                self.frames = np.random.randint(mn, mx, size=(nf, ny, nx), dtype=dt)
            else:
                # Use float32 for min/max, to prevent overflow errors
                dtinfo = np.finfo(np.float32)
                mn = dtinfo.min
                if minimum is not None:
                    mn = float(max(dtinfo.min, minimum))
                mx = dtinfo.max
                if maximum is not None:
                    mx = float(min(dtinfo.max, maximum))
                self.frames = np.random.uniform(mn, mx, size=(nf, ny, nx))
                if datatype == 'float32':
                    self.frames = np.float32(self.frames)

            print('Generated frame shape: {}'.format(self.frames[0].shape))
            print('Range of generated values: [{},{}]'.format(mn,mx))
        self.n_input_frames, self.rows, self.cols = self.frames.shape
        self.pva_type_key = self.PVA_TYPE_KEY_MAP.get(self.frames.dtype)
        print('Number of input frames: {} (size: {}x{}, type: {})'.format(self.n_input_frames, self.cols, self.rows, self.frames.dtype))

        self.channel_name = channel_name
        self.frame_rate = frame_rate
        self.server = pva.PvaServer()
        self.server.addRecord(self.channel_name, pva.NtNdArray())
        if notify_pv and notify_pv_value:
            try:
                time.sleep(self.NOTIFICATION_DELAY)
                notifyChannel = pva.Channel(notify_pv, pva.CA)
                notifyChannel.put(notify_pv_value)
                print('Set notification PV {} to {}'.format(notify_pv, notify_pv_value))
            except Exception as ex:
                print('Could not set notification PV {} to {}: {}'.format(notify_pv, notify_pv_value, ex))

        self.frame_map = {}
        self.current_frame_id = 0
        self.n_published_frames = 0
        self.start_time = 0
        self.last_published_time = 0
        self.start_delay = start_delay
        self.is_done = False

    def get_timestamp(self):
        s = time.time()
        ns = int((s-int(s))*1000000000)
        s = int(s)
        return pva.PvTimeStamp(s,ns)

    def frame_producer(self, extraFieldsPvObject=None):
        for frame_id in range(0, self.n_input_frames):
            if self.is_done:
               return

            if extraFieldsPvObject is None:
                nda = pva.NtNdArray()
            else:
                nda = pva.NtNdArray(extraFieldsPvObject.getStructureDict())

            data = self.frames[frame_id].flatten()
            nda['uniqueId'] = frame_id
            #nda['codec'] = pva.PvCodec('pvapyc', pva.PvInt(6))
            dims = [pva.PvDimension(self.cols, 0, self.cols, 1, False), \
                    pva.PvDimension(self.rows, 0, self.rows, 1, False)]
            nda['dimension'] = dims
            nda['compressedSize'] = self.rows*self.cols*data.itemsize
            nda['uncompressedSize'] = self.rows*self.cols*data.itemsize
            ts = self.get_timestamp()
            nda['timeStamp'] = ts
            nda['dataTimeStamp'] = ts
            nda['descriptor'] = 'PvaPy Simulated Image'
            nda['value'] = {self.pva_type_key : data}
            attrs = [pva.NtAttribute('ColorMode', pva.PvInt(0))]
            nda['attribute'] = attrs
            if extraFieldsPvObject is not None:
                nda.set(extraFieldsPvObject)
            self.frame_map[frame_id] = nda

    def prepare_frame(self):
        # Get cached frame
        cached_frame_id = self.current_frame_id % self.n_input_frames
        frame = self.frame_map[cached_frame_id]

        # Correct image id and timeestamps
        self.current_frame_id += 1
        frame['uniqueId'] = self.current_frame_id
        ts = self.get_timestamp()
        frame['timeStamp'] = ts
        frame['dataTimeStamp'] = ts
        return frame

    def frame_publisher(self):
        while True:
            if self.is_done:
                return

            frame = self.prepare_frame()
            self.server.update(self.channel_name, frame)
            self.last_published_time = time.time()
            self.n_published_frames += 1

            runtime = 0
            if self.n_published_frames > 1:
                runtime = self.last_published_time - self.start_time
                delta_t = runtime/(self.n_published_frames - 1)
                frame_rate = 1.0/delta_t
                if self.report_frequency > 0 and (self.n_published_frames % self.report_frequency) == 0:
                    print("Published frame id %6d @ %.3f (frame rate: %.4f fps)" % (self.current_frame_id, self.last_published_time, frame_rate))
            else:
                self.start_time = self.last_published_time
                if self.report_frequency > 0 and (self.n_published_frames % self.report_frequency) == 0:
                    print("Published frame id %6d @ %.3f" % (self.current_frame_id, self.last_published_time))

            if runtime > self.runtime:
                print("Server will exit after reaching runtime of {} seconds".format(self.runtime))
                return

            if self.delta_t > 0:
                next_publish_time = self.start_time + self.n_published_frames*self.delta_t
                delay = next_publish_time - time.time() - self.DELAY_CORRECTION
                if delay > 0:
                    threading.Timer(delay, self.frame_publisher).start()
                    return

    def start(self):
        threading.Thread(target=self.frame_producer, daemon=True).start()
        self.server.start()
        threading.Timer(self.start_delay, self.frame_publisher).start()

    def stop(self):
        self.is_done = True
        self.server.stop()
        runtime = self.last_published_time - self.start_time
        delta_t = runtime/(self.n_published_frames - 1)
        frame_rate = 1.0/delta_t
        print('\nServer runtime: %.4f seconds' % (runtime))
        print('Published frames: %6d @ %.4f fps' % (self.n_published_frames, frame_rate))

def main():
    parser = argparse.ArgumentParser(description='PvaPy Area Detector Simulator')
    parser.add_argument('--input-directory', '-id', type=str, dest='input_directory', default=None, help='Directory containing input files to be streamed; if input directory or input file are not provided, random images will be generated')
    parser.add_argument('--input-file', '-if', type=str, dest='input_file', default=None, help='Input file to be streamed; if input directory or input file are not provided, random images will be generated')
    parser.add_argument('--mmap-mode', '-mm', action='store_true', dest='mmap_mode', default=False, help='Use NumPy memory map to load the specified input file. This flag typically results in faster startup and lower memory usage for large files.')
    parser.add_argument('--frame-rate', '-fps', type=float, dest='frame_rate', default=20, help='Frames per second (default: 20 fps)')
    parser.add_argument('--n-x-pixels', '-nx', type=int, dest='n_x_pixels', default=2048, help='Number of pixels in x dimension (default: 256 pixels; does not apply if input file file is given)')
    parser.add_argument('--n-y-pixels', '-ny', type=int, dest='n_y_pixels', default=256, help='Number of pixels in x dimension (default: 256 pixels; does not apply if input file is given)')
    parser.add_argument('--datatype', '-dt', type=str, dest='datatype', default='uint8', help='Generated datatype. Possible options are int8, uint8, int16, uint16, int32, uint32, float32, float64 (default: uint8; does not apply if input file is given)')
    parser.add_argument('--minimum', '-mn', type=float, dest='minimum', default=None, help='Minimum generated value (does not apply if input file is given)')
    parser.add_argument('--maximum', '-mx', type=float, dest='maximum', default=None, help='Maximum generated value (does not apply if input file is given)')
    parser.add_argument('--n-frames', '-nf', type=int, dest='n_frames', default=1000, help='Number of different frames to generate and cache; those images will be published over and over again as long as the server is running')
    parser.add_argument('--runtime', '-rt', type=float, dest='runtime', default=300, help='Server runtime in seconds (default: 300 seconds)')
    parser.add_argument('--channel-name', '-cn', type=str, dest='channel_name', default='pvapy:image', help='Server PVA channel name (default: pvapy:image)')
    parser.add_argument('--notify-pv', '-npv', type=str, dest='notify_pv', default=None, help='CA channel that should be notified on start; for the default Area Detector PVA driver PV that controls image acquisition is 13PVA1:cam1:Acquire')
    parser.add_argument('--notify-pv-value', '-nvl', type=str, dest='notify_pv_value', default='1', help='Value for the notification channel; for the Area Detector PVA driver PV this should be set to "Acquire" (default: 1)')
    parser.add_argument('--start-delay', '-sd', type=float, dest='start_delay',  default=10.0, help='Server start delay in seconds (default: 10 seconds)')
    parser.add_argument('--report-frequency', '-rf', type=int, dest='report_frequency', default=1, help='Reporting frequency for publishing frames; if set to <=0 no frames will be reported as published (default: 1)')
    parser.add_argument('-v', '--version', action='version', version='%(prog)s {version}'.format(version=__version__))

    args, unparsed = parser.parse_known_args()
    if len(unparsed) > 0:
        print('Unrecognized argument(s): %s' % ' '.join(unparsed))
        exit(1)

    server = AdSimServer(input_directory=args.input_directory, input_file=args.input_file, mmap_mode=args.mmap_mode, frame_rate=args.frame_rate, nf=args.n_frames, nx=args.n_x_pixels, ny=args.n_y_pixels, datatype=args.datatype, minimum=args.minimum, maximum=args.maximum, runtime=args.runtime, channel_name=args.channel_name, notify_pv=args.notify_pv, notify_pv_value=args.notify_pv_value, start_delay=args.start_delay, report_frequency=args.report_frequency)

    server.start()
    try:
        runtime = args.runtime + 2*args.start_delay
        time.sleep(runtime)
    except KeyboardInterrupt as ex:
        pass
    server.stop()

if __name__ == '__main__':
    main()
