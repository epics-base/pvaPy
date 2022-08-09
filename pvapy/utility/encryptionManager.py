#!/usr/bin/env python

import pickle
import rsa
import rsa.randnum
from .aesCipher import AesCipher
from ..objects.encryptedData import EncryptedData
from ..objects.cipher import Cipher
from .loggingManager import LoggingManager
import pvaccess as pva

class EncryptionManager:
    ''' 
    Class that handles encryption and decryption of PvaPy objects.
    '''

    DEFAULT_KEY_LENGTH = 128
    DEFAULT_AES_MODE = AesCipher.DEFAULT_MODE
    DEFAULT_RSA_HASH_ALGORITHM = 'SHA-1'

    def __init__(self, privateKeyFilePath):
        self.logger = LoggingManager.getLogger(self.__class__.__name__)
        with open(privateKeyFilePath, mode='rb') as privateKeyFile:
            keydata = privateKeyFile.read()
        self.privateKey = rsa.PrivateKey.load_pkcs1(keydata)
        self.publicKey = rsa.PublicKey(self.privateKey.n, self.privateKey.e)

    def encrypt(self, pvObject, objectId, sign=False):
        return self.encryptWithAes(pvObject, objectId, sign)

    def encryptWithAes(self, pvObject, objectId, sign=False, keyLength=DEFAULT_KEY_LENGTH, mode=DEFAULT_AES_MODE):
        aesKey = rsa.randnum.read_random_bits(keyLength)
        encryptedAesKey = rsa.encrypt(aesKey, self.publicKey)

        pickledPvObject = pickle.dumps(pvObject)
        signature = ''
        if sign:
            signature = rsa.sign(pickledPvObject, self.privateKey, self.DEFAULT_RSA_HASH_ALGORITHM)
            self.logger.debug(f'Object {objectId} signed using {self.DEFAULT_RSA_HASH_ALGORITHM} hash')
        encryptedPvObject = AesCipher.encrypt(pickledPvObject, aesKey)
        cipherParameters = ( 
            {'value' : {'mode' : mode, 'keyLength' : keyLength}},
            {'value' : {'mode' : pva.USHORT, 'keyLength' : pva.USHORT}}
        )
        cipher = Cipher({'name' : 'AES'})
        cipher['parameters'] = cipherParameters
        return EncryptedData({
            'objectId' : int(objectId), 
            'data' : encryptedPvObject, 
            'key' : encryptedAesKey,
            'signature' : signature,
            'cipher' : cipher
        })

    def decryptWithAes(self, encryptedData, verify=False):
        objectId = encryptedData['objectId']
        encryptedAesKey = encryptedData['key']
        aesKey = rsa.decrypt(encryptedAesKey, self.privateKey)
        encryptedPvObject = encryptedData['data']
        pickledPvObject = AesCipher.decrypt(encryptedPvObject, aesKey)
        signature = encryptedData['signature']
        if verify:
            algorithm = rsa.verify(pickledPvObject, signature, self.publicKey)
            self.logger.debug(f'Object {objectId} signature verified using {algorithm} hash')
        pvObject = pickle.loads(pickledPvObject)
        return pvObject

    def decrypt(self, encryptedData, verify=False):
        cipher = encryptedData['cipher']
        cipherName = cipher['name']
        if cipherName != 'AES':
            raise pva.InvalidArgument(f'Unsupported cipher {cipherName}')
        return self.decryptWithAes(encryptedData, verify)

