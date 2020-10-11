Import("env")
from shutil import copyfile
import os


def after_bin(source, target, env):
    version = ""

    print("source: " + str(source[0]))
    print("target: " + str(target[0]))

    cppdefines = env.ParseFlags(env['BUILD_FLAGS']).get("CPPDEFINES")
    for arr in cppdefines:
        if len(arr) == 2:
            if arr[0] == "VERSION":
                version = arr[1]

    if version == "":
        version = "mqttSensorApp"

    path = "bin"
    srcFile = str(target[0])
    dstFile = os.path.join(path, version.strip('\\"') + ".bin")

    if not os.path.exists(path):
        os.makedirs(path)

    print("src: " + srcFile)
    print("dst: " + dstFile)
    copyfile(srcFile, dstFile)


print("Current build targets", map(str, BUILD_TARGETS))

env.AddPostAction("$BUILD_DIR/firmware.bin", after_bin)
