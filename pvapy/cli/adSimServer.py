#!/usr/bin/env python

import time, threading, queue, argparse
import numpy as np
import pvaccess as pva

__version__ = pva.__version__

class AdSimServer:

    def __init__(self, input_file, frame_rate, nf, nx, ny, runtime, channel_name, start_delay, report_frequency):
        self.arraySize = None
        self.delta_t = 1.0/frame_rate
        self.runtime = runtime
        self.report_frequency = report_frequency 
        
        if input_file is None:
            self.frames = np.random.randint(0, 256, size=(nf, nx, ny), dtype=np.uint8)
        else:
            import input_filepy
            with input_filepy.File(input_file, 'r') as input_filefd:
                self.frames = input_filefd['frames'][:]

        self.rows, self.cols = self.frames.shape[-2:]

        self.channel_name = channel_name
        self.n_generated_frames = nf
        self.frame_rate = frame_rate
        self.server = pva.PvaServer()
        self.server.addRecord(self.channel_name, pva.NtNdArray())
        self.frame_map = {}
        self.current_frame_id = 0
        self.n_published_frames = 0
        self.start_time = 0
        self.last_published_time = 0
        self.next_publish_time = 0
        self.start_delay = start_delay
        self.is_done = False

    def get_timestamp(self):
        s = time.time()
        ns = int((s-int(s))*1000000000)
        s = int(s)
        return pva.PvTimeStamp(s,ns)

    def frame_producer(self, extraFieldsPvObject=None):
        for frame_id in range(0, self.n_generated_frames):
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
            nda['value'] = {'ubyteValue': self.frames[frame_id].flatten()}
            attrs = [pva.NtAttribute('ColorMode', pva.PvInt(0))]
            nda['attribute'] = attrs
            if extraFieldsPvObject is not None:
                nda.set(extraFieldsPvObject)
            self.frame_map[frame_id] = nda

    def frame_publisher(self):
        if self.is_done:
            return
        entry_time = time.time()
        cached_frame_id = self.current_frame_id % self.n_generated_frames
        frame = self.frame_map[cached_frame_id]
        self.current_frame_id += 1
        frame['uniqueId'] = self.current_frame_id

        # Make sure we do not go too fast
        now = time.time()
        delay = (self.next_publish_time - now)*0.99
        if delay > 0:
            time.sleep(delay)
        self.server.update(self.channel_name, frame)
        self.last_published_time = time.time()
        self.next_publish_time = self.last_published_time + self.delta_t
        self.n_published_frames += 1

        runtime = 0
        delta_t_correction = 0
        if self.n_published_frames > 1:
            runtime = self.last_published_time - self.start_time
            delta_t = runtime/(self.n_published_frames - 1)

            # Attempt to correct rate with a bit of magic
            delta_t_correction = delta_t - self.delta_t
            if delta_t_correction > 0:
                delta_t_correction *= 10

            frame_rate = 1.0/delta_t
            if self.report_frequency > 0 and (self.n_published_frames % self.report_frequency) == 0:
                print("Published frame id %6d @ %.3f (frame rate: %.4f fps)" % (self.current_frame_id, self.last_published_time, frame_rate))
        else:
            self.start_time = self.last_published_time
            if self.report_frequency > 0 and (self.n_published_frames % self.report_frequency) == 0:
                print("Published frame id %6d @ %.3f" % (self.current_frame_id, self.last_published_time))
        if runtime > self.runtime:
            print("Server will exit after reaching runtime of %s seconds" % (self.runtime))
        else:
            delay = self.delta_t - (time.time()- entry_time) - delta_t_correction
            threading.Timer(delay, self.frame_publisher).start()

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
    parser.add_argument('--input-file', '-if', type=str, dest='input_file', default=None, help='Inpput hdf5 file to be streamed; if not provided, random image will be genrated')
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

    server = AdSimServer(input_file=args.input_file, frame_rate=args.frame_rate, nf=args.n_frames, nx=args.n_x_pixels, ny=args.n_y_pixels, runtime=args.runtime, channel_name=args.channel_name, start_delay=args.start_delay, report_frequency=args.report_frequency)

    server.start()
    try:
        runtime = args.runtime + 2*args.start_delay
        time.sleep(runtime)
    except KeyboardInterrupt as ex:
        pass
    server.stop()
    
if __name__ == '__main__':
    main()
