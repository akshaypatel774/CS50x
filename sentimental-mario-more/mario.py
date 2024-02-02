h = 0
while True:
    try:
        h = int(input("Height: "))
        if h in range(1, 9):
            break
    except ValueError:
        continue

for i in range(1, h + 1):
    print((" " * (h - i)) + ("#" * i) + "  " + ("#" * i))
