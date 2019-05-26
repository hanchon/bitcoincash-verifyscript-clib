import ctypes
from binascii import unhexlify

# txhash dab571fc05909d3b51c7698c0f1f8069b6aedec810c2d4ab35c2d42ede5da71f
tx = unhexlify("0200000001c4e30b2c60a7e5c570331c438f536d994b14b3e52f8e4ae5cfbf7389e1084ffc010000006a47304402203ea87520ddcd51f3b986ca5745b696793392439bf6c5b52a4696d2262ea6c0c302207dfe7c391c5b3ad864f25ca3628a98ef4d0a6a6de4e2ef8018e97bcce4554b8f4121020d01a18f138e0461e14fa3bac7913be2ad9e26b9a9fc4cfded431344ca2a0dcbffffffff0210270000000000001976a91428e1f3653ca13dc8d2b8a1fe81046ba730e4666f88acc8b16b01000000001976a91448374814c1f3c3138e2000d5d5e04bdedb25cf5088ac00000000")
script = unhexlify("76a91448374814c1f3c3138e2000d5d5e04bdedb25cf5088ac")
amount = 23854336
index = 0

res = ctypes.cdll.LoadLibrary('./build/src/libbitcoincash-consensus.so').verify_script_with_standard_script_verify_flags(
    tx, len(tx), script, len(script), index, amount
)

if res == 0:
    print ("Script success")
else:
    print ("Script error")