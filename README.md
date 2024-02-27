Implementation follows RFC 8439

poly1305-gen takes in a 32-byte key, given as a 64-character hexadecimal string (which will be split into r and s) and the name of the file to authenticate and will output the authenticator tag.
poly1305-test takes the same key as poly1305-gen, the file to authenticate and an authenticator tag. It will output ACCEPT or REJECT.
