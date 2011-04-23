import socket


class TicketeerClient(object):
    def __init__(self, ip = None):
        if ip is None:
            ip = "127.0.0.1"
        self.ip = ip
        self.port = 21212
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def ticket(self, ticket_space):
        address = (self.ip, self.port)
        self.sock.sendto(ticket_space +"\n", address)
        (output, return_address) = self.sock.recvfrom(80)
        return output.strip()
