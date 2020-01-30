
import vfs

# python callback
def py_callback(i, s):
    print('py_callback(%d, %s)'%(i, s))

vfs.use_callback(py_callback)