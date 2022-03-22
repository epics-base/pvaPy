#!/usr/bin/env python

import time, threading, queue, argparse
import numpy as np
import pvaccess as pva
import os, os.path

__version__ = pva.__version__

class AdSimServer:

    DELAY_CORRECTION = 0.0001
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

    def __init__(self, input_directory, input_file, frame_rate, nf, nx, ny, runtime, channel_name, start_delay, report_frequency):
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
                new_frames = np.load(f)
                if self.frames is None:
                    self.frames = new_frames
                else:
                    self.frames = np.append(self.frames, new_frames, axis=0)
                print('Loaded input file %s' % (f))
            except Exception as ex:
                print('Cannot load input file %s, skipping it: %s' % (f, ex))
        if self.frames is None:
            print('Generating random frames')
            self.frames = np.random.randint(0, 256, size=(nf, nx, ny), dtype=np.uint8)
        self.n_input_frames, self.rows, self.cols = self.frames.shape
        self.pva_type_key = self.PVA_TYPE_KEY_MAP.get(self.frames.dtype)
        print('Number of input frames: %s (size: %sx%s, type: %s)' % (self.n_input_frames, self.rows, self.cols, self.frames.dtype))

        self.channel_name = channel_name
        self.frame_rate = frame_rate
        self.server = pva.PvaServer()
        self.server.addRecord(self.channel_name, pva.NtNdArray())
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

            nda['uniqueId'] = frame_id
            nda['codec'] = pva.PvCodec('pvapyc', pva.PvInt(5))
            dims = [pva.PvDimension(self.rows, 0, self.rows, 1, False), \
                    pva.PvDimension(self.cols, 0, self.cols, 1, False)]
            nda['dimension'] = dims
            nda['compressedSize'] = self.rows*self.cols
            nda['uncompressedSize'] = self.rows*self.cols
            ts = self.get_timestamp()
            nda['timeStamp'] = ts
            nda['dataTimeStamp'] = ts
            nda['descriptor'] = 'PvaPy Simulated Image'
            nda['value'] = {self.pva_type_key : self.frames[frame_id].flatten()}
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
                print("Server will exit after reaching runtime of %s seconds" % (self.runtime))
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
    parser.add_argument('--frame-rate', '-fps', type=float, dest='frame_rate', default=20, help='Frames per second (default: 20 fps)')
    parser.add_argument('--n-x-pixels', '-nx', type=int, dest='n_x_pixels', default=2048, help='Number of pixels in x dimension (default: 256 pixels; does not apply if input_file file is given)')
    parser.add_argument('--n-y-pixels', '-ny', type=int, dest='n_y_pixels', default=256, help='Number of pixels in x dimension (default: 256 pixels; does not apply if hdf5 file is given)')
    parser.add_argument('--n-frames', '-nf', type=int, dest='n_frames', default=1000, help='Number of different frames to generate and cache; those images will be published over and over again as long as the server is running')
    parser.add_argument('--runtime', '-rt', type=float, dest='runtime', default=300, help='Server runtime in seconds (default: 300 seconds)')
    parser.add_argument('--channel-name', '-cn', type=str, dest='channel_name', default='pvapy:image', help='Server PVA channel name (default: pvapy:image)')
    parser.add_argument('--start-delay', '-sd', type=float, dest='start_delay',  default=10.0, help='Server start delay in seconds (default: 10 seconds)')
    parser.add_argument('--report-frequency', '-rf', type=int, dest='report_frequency', default=1, help='Reporting frequency for publishing frames; if set to <=0 no frames will be reported as published (default: 1)')
    parser.add_argument('-v', '--version', action='version', version='%(prog)s {version}'.format(version=__version__))

    args, unparsed = parser.parse_known_args()
    if len(unparsed) > 0:
        print('Unrecognized argument(s): %s' % ' '.join(unparsed))
        exit(1)

    server = AdSimServer(input_directory=args.input_directory, input_file=args.input_file, frame_rate=args.frame_rate, nf=args.n_frames, nx=args.n_x_pixels, ny=args.n_y_pixels, runtime=args.runtime, channel_name=args.channel_name, start_delay=args.start_delay, report_frequency=args.report_frequency)

    server.start()
    try:
        runtime = args.runtime + 2*args.start_delay
        time.sleep(runtime)
    except KeyboardInterrupt as ex:
        pass
    server.stop()
    
if __name__ == '__main__':
    main()
