#!/usr/bin/env python

import pickle
import rsa
import rsa.randnum
from .aesCipher import AesCipher
from ..objects.encryptedData import EncryptedData
from ..objects.cipher import Cipher
import pvaccess as pva

class EncryptionManager:
    ''' 
    Class that handles encryption and decryption of PvaPy objects.
    '''

    DEFAULT_KEY_LENGTH = 256
    DEFAULT_AES_MODE = AesCipher.DEFAULT_MODE

    def __init__(self, privateKeyFilePath):
        with open(privateKeyFilePath, mode='rb') as privateKeyFile:
            keydata = privateKeyFile.read()
        self.privateKey = rsa.PrivateKey.load_pkcs1(keydata)
        self.publicKey = rsa.PublicKey(self.privateKey.n, self.privateKey.e)

    def encrypt(self, pvObject, objectId):
        return self.encryptWithAes(pvObject, objectId)

    def encryptWithAes(self, pvObject, objectId, keyLength=DEFAULT_KEY_LENGTH, mode=DEFAULT_AES_MODE):
        aesKey = rsa.randnum.read_random_bits(keyLength)
        encryptedAesKey = rsa.encrypt(aesKey, self.publicKey)

        pickledPvObject = pickle.dumps(pvObject)
        encryptedPvObject = AesCipher.encrypt(pickledPvObject, aesKey)
        cipherParameters = [
            pva.PvObject({'mode':pva.INT}, {'mode':mode}),
            pva.PvObject({'keyLength':pva.USHORT}, {'keyLength':keyLength})
        ]
        return EncryptedData({
            'objectId' : int(objectId), 
            'data' : encryptedPvObject, 
            'key' : encryptedAesKey,
            'cipher' : Cipher({'name' : 'AES', 'parameters' : cipherParameters})
        })

    def decryptWithAes(self, encryptedData):
        encryptedAesKey = encryptedData['key']
        aesKey = rsa.decrypt(encryptedAesKey, self.privateKey)
        encryptedPvObject = encryptedData['data']
        pickledPvObject = AesCipher.decrypt(encryptedPvObject, aesKey)
        pvObject = pickle.loads(pickledPvObject)
        return pvObject

    def decrypt(self, encryptedData):
        cipher = encryptedData['cipher']
        cipherName = cipher['name']
        if cipherName != 'AES':
            raise pva.InvalidArgument(f'Unsupported cipher {cipherName}')
        return self.decryptWithAes(encryptedData)

