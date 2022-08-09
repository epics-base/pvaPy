import base64
import hashlib
from Crypto.Cipher import AES
import os

class AesCipher:
    ''' 
    Class that handles AES encryption. 
    '''

    DEFAULT_MODE = AES.MODE_EAX

    @classmethod
    def encode(cls, s):
        # Convert to bytes if needed
        if type(s) == bytes:
            return s
        return s.encode('utf-8')

    @classmethod
    def pad(cls, s):
        # Pads with pad size integer
        bs = AES.block_size
        ps = bs - len(s)%bs
        return cls.encode(s) + cls.encode(ps*chr(ps))

    @classmethod
    def unpad(cls, s):
        # Input will always be bytes, last character will indicate padding
        offset = s[-1]
        return s[:-offset]

    @classmethod
    def encrypt(cls, plainText, key, mode=DEFAULT_MODE):
        privateKey = hashlib.sha256(cls.encode(key)).digest()
        paddedText = cls.pad(plainText)
        iv = os.urandom(AES.block_size)
        cipher = AES.new(privateKey, mode, iv)
        return base64.b64encode(iv + cipher.encrypt(paddedText))

    @classmethod
    def decrypt(cls, cipherText, key, mode=DEFAULT_MODE):
        privateKey = hashlib.sha256(cls.encode(key)).digest()
        cipherText = base64.b64decode(cipherText)
        iv = cipherText[:AES.block_size]
        cipher = AES.new(privateKey, mode, iv)
        return cls.unpad(cipher.decrypt(cipherText[AES.block_size:]))

