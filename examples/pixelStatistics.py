#!/usr/bin/env python

from collections.abc import Mapping, Sequence
from typing import Any, Final
import ctypes
import ctypes.util
import os
import tempfile
import time

import numpy

from pvapy.hpc.adImageProcessor import AdImageProcessor
from pvapy.utility.floatWithUnits import FloatWithUnits
import pvaccess as pva


def find_epics_db() -> None:
    if not os.environ.get('EPICS_DB_INCLUDE_PATH'):
        pvDataLib = ctypes.util.find_library('pvData')

        if pvDataLib:
            pvDataLib = os.path.realpath(pvDataLib)
            epicsLibDir = os.path.dirname(pvDataLib)
            dbdDir = os.path.realpath(f'{epicsLibDir}/../../dbd')
            os.environ['EPICS_DB_INCLUDE_PATH'] = dbdDir
        else:
            raise Exception('Cannot find dbd directory, please set EPICS_DB_INCLUDE_PATH'
                            'environment variable')


def create_ca_ioc(pvseq: Sequence[str]) -> pva.CaIoc:
    # create database and start IOC
    dbFile = tempfile.NamedTemporaryFile(delete=False)
    dbFile.write(b'record(ao, "$(NAME)") {}\n')
    dbFile.close()

    ca_ioc = pva.CaIoc()
    ca_ioc.loadDatabase('base.dbd', '', '')
    ca_ioc.registerRecordDeviceDriver()

    for pv in pvseq:
        print(f'Creating CA ca record: {pv}')
        ca_ioc.loadRecords(dbFile.name, f'NAME={pv}')

    ca_ioc.start()
    os.unlink(dbFile.name)
    return ca_ioc


class PixelStatisticsProcessor(AdImageProcessor):
    DESAT_PV: Final[str] = 'pvapy:desat'
    DESAT_KW: Final[str] = 'desat_threshold'
    SAT_PV: Final[str] = 'pvapy:sat'
    SAT_KW: Final[str] = 'sat_threshold'
    SUM_PV: Final[str] = 'pvapy:sum'

    def __init__(self, config_dict: Mapping[str, Any] = {}) -> None:
        super().__init__(config_dict)
        find_epics_db()

        self._desat_threshold = config_dict.get(self.DESAT_KW, 1)
        self._sat_threshold = config_dict.get(self.SAT_KW, 254)
        self._ca_ioc = pva.CaIoc()

        # statistics
        self.num_frames_processed = 0
        self.processing_time_s = 0

    def start(self) -> None:
        self._ca_ioc = create_ca_ioc([self.DESAT_PV, self.SAT_PV, self.SUM_PV])
        self.logger.debug(self._ca_ioc.getRecordNames())

    def configure(self, config_dict: Mapping[str, Any]) -> None:
        try:
            self._desat_threshold = int(config_dict[self.DESAT_KW])
        except KeyError:
            pass
        except ValueError:
            self.logger.warning('Failed to parse desaturation threshold!')
        else:
            self.logger.debug(f'Desaturation threshold: {self._desat_threshold}')

        try:
            self._sat_threshold = int(config_dict[self.SAT_KW])
        except KeyError:
            pass
        except ValueError:
            self.logger.warning('Failed to parse saturation threshold!')
        else:
            self.logger.debug(f'Saturation threshold: {self._sat_threshold}')

    def process(self, pvObject: pva.PvObject) -> pva.PvObject:
        t0 = time.time()

        (frameId, image, nx, ny, nz, colorMode, fieldKey) = self.reshapeNtNdArray(pvObject)

        if nx is None:
            self.logger.debug(f'Frame id {frameId} contains an empty image.')
            return pvObject

        desat_pixels = numpy.count_nonzero(image < self._desat_threshold)
        self._ca_ioc.putField(self.DESAT_PV, desat_pixels)

        sat_pixels = numpy.count_nonzero(image > self._sat_threshold)
        self._ca_ioc.putField(self.SAT_PV, sat_pixels)

        sum_pixels = image.sum()
        self._ca_ioc.putField(self.SUM_PV, sum_pixels)

        t1 = time.time()
        self.processing_time_s += (t1 - t0)

        return pvObject

    def stop(self) -> None:
        pass

    def resetStats(self) -> None:
        self.num_frames_processed = 0
        self.processing_time_s = 0

    def getStats(self) -> Mapping[str, Any]:
        processed_frame_rate_Hz = 0

        if self.processing_time_s > 0:
            processed_frame_rate_Hz = self.num_frames_processed / self.processing_time_s

        return {
            'num_frames_processed': self.num_frames_processed,
            'processing_time_s': FloatWithUnits(self.processing_time_s, 's'),
            'processed_frame_rate_Hz': FloatWithUnits(processed_frame_rate_Hz, 'fps'),
        }

    def getStatsPvaTypes(self) -> Mapping[str, Any]:
        return {
            'num_frames_processed': pva.UINT,
            'processing_time_s': pva.DOUBLE,
            'processed_frame_rate_Hz': pva.DOUBLE,
        }
