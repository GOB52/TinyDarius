# rename from firmware.bin to projectname.bin
# 
# platform.ini
# extra_scripts = pre:rename_bin.py
#
Import("env")
import os

project_name = os.path.basename(os.path.dirname(env["PROJECT_CONFIG"]))

env.Replace(PROGNAME="%s" % project_name)

#
# If you want to rename to project name + env name then use it.
#
#env.Replace(PROGNAME="%s_%s" % (project_name, env["PIOENV"]));


       
