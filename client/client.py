from oasis_protocol import Oasis
from principal_interface import PrincipalMenu

SERVER_IP = "127.0.0.1"
SERVER_PORT = 8080

def main():

    Oasis_protocol = Oasis(SERVER_IP, SERVER_PORT)
    app = PrincipalMenu(Oasis_protocol)
    app.mainloop()

if __name__ == '__main__':
    main()