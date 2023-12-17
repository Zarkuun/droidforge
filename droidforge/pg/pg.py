# ----------------------------------------------------------------
# BEWARE!
#
# This file has been created by the DROID Forge. It is overwritten
# every time you start the Forge! If you want to edit this file,
# you need to create a copy with a different name here. Otherwise
# your changes will be lost.
# ----------------------------------------------------------------

# Here are some useful functions that can be used by any
# patch generator

import sys
import json

# Collects labels for jacks and controls here
labels = {}

def set_synopsis(syn):
    # Sorry for the global variable. Makes our live easier
    global synopsis
    global default_preset
    synopsis = syn
    default_preset = synopsis["presets"][0]
    check_presets()
    args = parse_options()
    return collect_arguments(args)

def parse_options():
    global default_preset
    args = sys.argv[1:]
    while args and args[0].startswith("-"):
        if args[0] in [ '-p', '--preset' ]:
            del args[0]
            if not args:
                bail_out("The option -p needs an argument.")
            preset_name = args[0]
            del args[0]
            for preset in synopsis["presets"]:
                if preset["name"] == preset_name:
                    default_preset = preset
                    break
            else:
                bail_out("There is no preset '%s'" % preset_name)
        else:
            parse_option(args[0])
            del args[0]
    return args

def parse_option(option):
    if option in [ '-h', '--help']:
        help_and_exit()
    elif option in [ '-s', '--synopsis']:
        show_synopsis_and_exit()
    else:
        bail_out("Unknown option %s." % option)

def show_synopsis_and_exit():
    sys.stdout.write(json.dumps(synopsis))
    sys.stdout.write("\n")
    sys.exit(0)

def default_arguments():
    return default_preset["parameters"].items()

def help_and_exit():
    print("DROID patch generator \"%s\"" % synopsis["title"])
    print()
    print(synopsis["description"])
    print()
    print("EXAMPLE:")
    print("    %s m=master18 tracks=3 > sequencer.ini" % sys.argv[0])
    print()
    print("OPTIONS:")
    print()
    print("    -h, --help:     show this help")
    print("    -s, --synopsis: output possible parameters as JSON")
    print("    -p, --preset P  use settings from preset P")
    print()
    print("    Available presets:")
    print()
    for preset in synopsis["presets"]:
        print("      %-12s    %s" % (preset["name"], preset["title"]))
    print()

    print("PARAMETERS (defaults are marked with a *):")


    def_args_dict = dict(default_arguments())
    for section in synopsis["sections"]:
        #  print("%s:" % section["title"].upper())
        for option in section["options"]:
            print()

            if "enum" in option:
                print("    %s:" % option["title"])
                for name, title in option["enum"]:
                    opt = "%s=%s:" % (option["name"], name)
                    if def_args_dict[option["name"]] == name:
                        defstar = "* "
                    else:
                        defstar = "  "
                    print("       %s%-14s %s" % (defstar, opt, title))

            elif "number" in option:
                lowest, highest = option["number"]
                print("    %s (%d-%d):" % (option["title"], lowest, highest))
                default = def_args_dict[option["name"]]
                last_dots = False
                for num in range(lowest, highest+1):
                    opt = "%s=%d:" % (option["name"], num)
                    if default == num:
                        defstar = "* "
                    else:
                        defstar = "  "
                    if num in [ lowest, default, highest ]:
                        print("       %s%-14s %s is %d" % (defstar, opt, option["title"], num))
                        last_dots = False
                    elif not last_dots:
                        last_dots = True

            else: # bool
                print("    %s:" % option["title"])
                default = def_args_dict[option["name"]]
                print("       %s%-14s %s" % ("  " if default else "* ", option["name"] + "=0",
                    "Don't " + option["title"][0].lower() + option["title"][1:]))
                print("       %s%-14s %s" % ("* " if default else "  ", option["name"] + "=1",
                    option["title"]))

    print()
    sys.exit(0)

def circuit(name, params, comment = None):
    patch = "\n"
    if comment:
        words = comment.split()
        while words:
            line = ""
            while words and len(line) < 90:
                line += words[0] + " "
                del words[0]
            line = line.strip() + "\n"
            patch += "# " + line
    patch += "[%s]\n" % name
    for param, value in params:
        patch += "    %s = %s\n" % (param, value)
    return patch

def find_option(name):
    for section in synopsis["sections"]:
        for option in section["options"]:
            if option["name"] == name:
                return  option

def bail_out(reason):
    sys.stdout.write(reason + "\n")
    sys.exit(1)

def collect_arguments(args):
    arguments = {}
    arguments.update(default_arguments())
    for arg in args:
        parts = arg.split("=")
        if len(parts) != 2:
            bail_out("Invalid parameter '%s': must contain one =" % arg)
        name, value = parts
        option = find_option(name)
        if not option:
            bail_out("Unknown parameter '%s'" % name)

        if "enum" in option:
            allowed = [ e[0] for e in option["enum"] ]
            if value not in allowed:
                bail_out("Invalid value %s=%s. Allowed are %s." % (
                    name, value, ", ".join(allowed)))
            arguments[name] = value

        elif "number" in option:
            try:
                intval = int(value)
            except:
                bail_out("Invalid value %s=%s: %s is not a number." % (
                    name, value, value))
            lowest, highest = option["number"]
            if intval < lowest or intval > highest:
                bail_out("Invalid value for '%s': %d it is out of the allowed range of %d ... %d." % (
                    option["title"], intval, lowest, highest))
            arguments[name] = intval

        else: # boolean
            if value not in [ "0", "1" ]:
                bail_out("Invalid value %s=%s. Allowed are 0 and 1." % (name, value))
            arguments[name] = value == "1"

    return arguments

def check_presets():
    for preset in synopsis["presets"]:
        for section in synopsis["sections"]:
            for option in section["options"]:
                if option["name"] not in preset["parameters"]:
                    bail_out("Preset '%s' is missing option '%s'." % (
                        preset["name"], option["name"]))
                value = preset["parameters"][option["name"]]

                if "enum" in option:
                    if value not in [ a[0] for a in option["enum"]]:
                        bail_out("Preset '%s' has invalid value %r for option '%s'." % (
                            preset["name"], value, option["name"]))

                elif "number" in option:
                    if (type(value) != int or
                        value < option["number"][0] or
                        value > option["number"][1]):
                        bail_out("Preset '%s' has out of range number %r for option '%s'." % (
                            preset["name"], value, option["name"]))

                elif value not in [ True, False ]:
                    bail_out("Preset '%s' has value %r for option '%s', but must be True or False." % (
                        preset["name"], value, option["name"]))

def add_label(jack, short, long):
    labels[jack] = (short, long)
    return jack

def patch_labels():
    patch = "\n"
    for jack, (short, long) in sorted(list(labels.items())):
        patch += "# %s: [%s] %s\n" % (jack, short, long)
    patch += "\n"
    return patch
