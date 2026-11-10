from clean_blog import *
from common import *
import sys

mode = sys.argv[1]
path = sys.argv[2]

clean_func = None

if mode == "blog":
    clean_func = clean_blog
else:
    exit(1)

clean_dir_recurse(path, clean_func)