def main():
    card = get_card()
    sum = check_sum(card)
    card_type(card, sum)


def get_card():
    while True:
        card = input("Number: ")
        if card.isnumeric():
            break

    return card


def check_sum(card):
    sum = 0
    for i, digit in enumerate(reversed(card)):
        if i % 2 != 0:
            if int(digit) * 2 > 9:
                sum += int(digit) * 2 - 9
            else:
                sum += int(digit) * 2
        else:
            sum += int(digit)
    return sum % 10 == 0


def card_type(card, sum):
    if sum:
        if (int(card[0]) == 4) and (len(card) == 13 or len(card) == 16):
            print("VISA")
        elif (int(card[0:2]) == 34 or int(card[0:2]) == 37) and len(card) == 15:
            print("AMEX")
        elif int(card[0:2]) > 50 and int(card[0:2]) < 56:
            print("MASTERCARD")
        else:
            print("INVALID")
    else:
        print("INVALID")


main()
