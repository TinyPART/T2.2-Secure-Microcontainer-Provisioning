from riot import saul

def get_temperature():
    reg = saul.get_registry()
    for device in reg:
        if device.type() == saul.SENSE_TEMP:
            print("{}: {}".format(device, device.read()))
            return int(device.read()[0] * 100)
