HEART_RATE_MIN = 80
HEART_RATE_MAX = 150

heart_rate = HEART_RATE_MIN
direction = 2

def get_heart_rate():
    global direction
    global heart_rate
    print(heart_rate)
    if heart_rate == HEART_RATE_MIN:
        direction = 2
    elif heart_rate == HEART_RATE_MAX:
        direction = -2
    heart_rate = heart_rate + direction
    return heart_rate
