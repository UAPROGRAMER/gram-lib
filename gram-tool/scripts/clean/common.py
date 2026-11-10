import os

def clean_file(file_path: str, clean_func: function):
    data = None

    err = False

    with open(file_path, "r") as in_file:
        try:
            data = in_file.read()
        except:
            err = True

    if err:
        os.remove(file_path)
        return

    data = clean_func(data)

    with open(file_path, "w") as out_file:
        out_file.write(data)

def clean_dir_recurse(dir_path: str, clean_func: function):
    for _, subpaths, files in os.walk(dir_path):
        for subpath in subpaths:
            clean_dir_recurse(os.path.join(dir_path, subpath), clean_func)
        for file_path in files:
            clean_file(os.path.join(dir_path, file_path), clean_func)