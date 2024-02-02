import re


def main():
    text = input("Text: ")
    words = len(text.split())
    l = (len(re.findall("[a-zA-Z]", text)) * 100) / words
    s = ((len(re.split("[.!?]", text)) - 1) * 100) / words

    index = 0.0588 * l - 0.296 * s - 15.8

    if index < 1:
        print("Before Grade 1")
    elif round(index) in range(1, 16):
        print(f"Grade {round(index)}")
    else:
        print("Grade 16+")


main()
