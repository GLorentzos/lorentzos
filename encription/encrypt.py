import hashlib
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
import base64
import os
from cryptography.hazmat.backends import default_backend  # Correct import

# Password for encryption
PASSWORD = "1234"

# SHA-256 Hashing (equivalent to SHA5 hash request)
def sha256_hash(text):
    sha_signature = hashlib.sha256(text.encode()).hexdigest()
    return sha_signature

# RSA Public/Private Key Generation with Password Encryption for Private Key
def generate_rsa_keys(password):
    private_key = rsa.generate_private_key(
        public_exponent=65537,
        key_size=2048
    )
    public_key = private_key.public_key()

    # Generate salt for password-based encryption
    salt = os.urandom(16)

    # Derive encryption key from the password using PBKDF2
    kdf = PBKDF2HMAC(
        algorithm=hashes.SHA256(),
        length=32,
        salt=salt,
        iterations=100000,
        backend=default_backend()  # Correct use of default_backend
    )
    encryption_key = kdf.derive(password.encode())

    # Encrypt the private key using the derived encryption key
    private_pem = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.TraditionalOpenSSL,
        encryption_algorithm=serialization.BestAvailableEncryption(encryption_key)
    )

    public_pem = public_key.public_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PublicFormat.SubjectPublicKeyInfo
    )

    # Save the salt to a file (salt will be needed for decryption)
    with open("salt.bin", "wb") as salt_file:
        salt_file.write(salt)

    return private_pem, public_pem, salt

# Encrypt Text with Public Key
def encrypt_with_public_key(public_key_pem, text_to_encrypt):
    public_key = serialization.load_pem_public_key(public_key_pem)
    
    encrypted = public_key.encrypt(
        text_to_encrypt.encode(),
        padding.OAEP(
            mgf=padding.MGF1(algorithm=hashes.SHA256()),
            algorithm=hashes.SHA256(),
            label=None
        )
    )
    return base64.b64encode(encrypted).decode()

# Decrypt Text with Private Key (and password)
def decrypt_with_private_key(private_key_pem, encrypted_text, password, salt):
    # Derive encryption key from the password using PBKDF2
    kdf = PBKDF2HMAC(
        algorithm=hashes.SHA256(),
        length=32,
        salt=salt,
        iterations=100000,
        backend=default_backend()  # Correct use of default_backend
    )
    encryption_key = kdf.derive(password.encode())

    # Load private key with password decryption
    private_key = serialization.load_pem_private_key(private_key_pem, password=encryption_key)
    
    encrypted_data = base64.b64decode(encrypted_text)

    decrypted = private_key.decrypt(
        encrypted_data,
        padding.OAEP(
            mgf=padding.MGF1(algorithm=hashes.SHA256()),
            algorithm=hashes.SHA256(),
            label=None
        )
    )
    return decrypted.decode()

# Example of Usage
if __name__ == "__main__":
    text_to_encrypt = "o akis einai loco"
    
    # Step 1: Generate RSA Keys (with password for private key)
    private_key, public_key, salt = generate_rsa_keys(PASSWORD)

    # Save the private key to a file
    with open("private_key.pem", "wb") as private_file:
        private_file.write(private_key)
    print("Private key saved as private_key.pem")

    # Save the public key to a file
    with open("public_key.pem", "wb") as public_file:
        public_file.write(public_key)
    print("Public key saved as public_key.pem")

    # Step 2: Encrypt the Text using Public Key
    encrypted_text = encrypt_with_public_key(public_key, text_to_encrypt)
    print(f"\nEncrypted Text: {encrypted_text}")
    
    # Step 3: Decrypt the Text using Private Key and Password
    # Load the private key from the file
    with open("private_key.pem", "rb") as private_file:
        private_key_from_file = private_file.read()

    # Load the salt from the file
    with open("salt.bin", "rb") as salt_file:
        salt = salt_file.read()
