#!/usr/bin/env python3
"""Send malformed UDP frames to the C++ gateway for fault-injection testing.

This tool intentionally sends truncated, random and size-correct corrupted packets.
It is used to verify that the gateway rejects bad data without crashing.
"""
import argparse
import os
import random
import socket
import time

WIRE_SIZE = 64


def send(sock: socket.socket, host: str, port: int, payload: bytes, label: str) -> None:
    sock.sendto(payload, (host, port))
    print(f"sent {label}: {len(payload)} bytes")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=46000)
    parser.add_argument("--count", type=int, default=30)
    parser.add_argument("--delay", type=float, default=0.05)
    args = parser.parse_args()

    rng = random.Random(42)
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        for i in range(args.count):
            mode = i % 3
            if mode == 0:
                payload = os.urandom(rng.randint(1, WIRE_SIZE - 1))
                label = "truncated_random"
            elif mode == 1:
                payload = os.urandom(WIRE_SIZE)
                label = "size_correct_random_crc_fail"
            else:
                payload = b"SG" + os.urandom(WIRE_SIZE - 2)
                label = "bad_protocol_fields"
            send(sock, args.host, args.port, payload, label)
            time.sleep(args.delay)


if __name__ == "__main__":
    main()
