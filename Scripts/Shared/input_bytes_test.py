import sys

def input_bytes():
    queryString = "Enter the numbe of bytes to record:\n"
    errorMessage = "Not a valid number.\n"

    userInput = raw_input(queryString)
    try:
        numBytes = int(userInput)
    except ValueError:
        print(errorMessage)
        return input_bytes()
    return numBytes

if __name__ == '__main__':
    numBytes = input_bytes()
    print numBytes
