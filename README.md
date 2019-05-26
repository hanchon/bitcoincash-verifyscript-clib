# Bitcoin cash verify script lib

This lib uses the [bitcoin-abc](https://github.com/bitcoin-abc/bitcoin-abc) files to export a function that verifies a transaction signature.
It's similar to the [libbitcoin-consensus](https://github.com/libbitcoin/libbitcoin-consensus) lib.

# Requirements:

*NOTE: It was only tested on debian.*

## Build requirements:

```
sudo apt-get install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils

sudo apt-get install libboost-all-dev
```

## Build the lib
```
git clone https://github.com/hanchon/bitcoincash-verifyscript-lib
cd bitcoincash-verifyscript-lib
./build.sh
```

The .so file will be in the build folder.

## Test the lib

Build the lib and run the python example.

```
python example.py
```


# Functions
The function `verify_script` needs the **transaction** in bytes, the transaction **length**, the previous **script** in bytes, the previous script **length**, the input **index** that is going to be validated, the **flags** and the previous output **amount**.

The function `verify_script_with_standard_script_verify_flags` calls the `verify_script` function using the current standard flags.

The return is 0 if the script is OK, any other result is an error (it returns the same enum that bitcoin-abc uses).


# License

[MIT license](https://opensource.org/licenses/MIT.)
