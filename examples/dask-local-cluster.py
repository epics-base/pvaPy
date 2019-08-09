#!/usr/bin/env python

from distributed import LocalCluster
import time

N_WORKERS = 12
SCHEDULER_PORT = 33333

if __name__ == '__main__':
    print('Start Cluster')
    cluster = LocalCluster(n_workers=N_WORKERS, scheduler_port=SCHEDULER_PORT)
    print(cluster)
    print('Cluster Running')
    time.sleep(1000)
