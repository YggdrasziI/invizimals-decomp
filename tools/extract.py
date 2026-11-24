#!/usr/bin/env python3
"""Extract the target module from the game ISO, decrypt it and check its SHA-1.

The ISO is read with 7-Zip (`7z` in PATH or the default install folder). On this
disc SYSDIR/BOOT.BIN is zeroed, so the target is SYSDIR/EBOOT.BIN, decrypted with
pspdecrypt (downloaded to tools/pspdecrypt/ by tools/psp/download_tool.py, never
committed: notes/PROVENANCE.md). Everything goes to orig/<version>/, git-ignored.
"""

import argparse
import hashlib
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PSPDECRYPT_TAG = "1.0"  # John-K/pspdecrypt release
PSPDECRYPT_DIR = ROOT / "tools" / "pspdecrypt"

# Inputs per version: ISO SHA-1, then encrypted file in the ISO -> decrypted name
VERSIONS = {
    "UCES01241": {
        "iso_sha1": "5a0a24e2d45db3da573a97ae1a86ce3ea59f6fa7",
        "files": {"PSP_GAME/SYSDIR/EBOOT.BIN": "EBOOT.PRX"},
    },
}


def sha1_of(path: Path) -> str:
    h = hashlib.sha1()
    with path.open("rb") as f:
        while chunk := f.read(1 << 20):
            h.update(chunk)
    return h.hexdigest()


def find_7z() -> str:
    for name in ("7z", "7zz"):
        if exe := shutil.which(name):
            return exe
    default = Path("C:/Program Files/7-Zip/7z.exe")
    if default.exists():
        return str(default)
    sys.exit("7z not found: install 7-Zip or add it to PATH")


def find_pspdecrypt() -> Path:
    exe = PSPDECRYPT_DIR / ("pspdecrypt.exe" if sys.platform == "win32" else "pspdecrypt")
    if not exe.exists():
        subprocess.run([sys.executable, str(ROOT / "tools" / "psp" / "download_tool.py"),
                        "pspdecrypt", PSPDECRYPT_TAG, "--path", str(PSPDECRYPT_DIR)], check=True)
    return exe


def expected_sha1(version: str) -> dict[str, str]:
    """Read config/<version>/build.sha1 (sha1sum format)."""
    result = {}
    sha1_file = ROOT / "config" / version / "build.sha1"
    if sha1_file.exists():
        for line in sha1_file.read_text().splitlines():
            if line.strip():
                digest, name = line.split(maxsplit=1)
                result[Path(name.lstrip("*")).name] = digest
    return result


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-v", "--version", default="UCES01241", choices=VERSIONS)
    parser.add_argument("--iso", type=Path, help="ISO path (default: the only .iso in orig/<version>/)")
    parser.add_argument("--skip-iso-check", action="store_true", help="Do not hash the ISO (slow)")
    args = parser.parse_args()

    orig = ROOT / "orig" / args.version
    iso = args.iso
    if iso is None:
        isos = sorted(orig.glob("*.iso"))
        if len(isos) != 1:
            sys.exit(f"put exactly one .iso in {orig.relative_to(ROOT)} (found {len(isos)})")
        iso = isos[0]

    info = VERSIONS[args.version]
    if not args.skip_iso_check:
        digest = sha1_of(iso)
        if digest != info["iso_sha1"]:
            sys.exit(f"{iso.name}: SHA-1 {digest}, expected {info['iso_sha1']}")
        print(f"{iso.name}: OK")

    expected = expected_sha1(args.version)
    seven_zip = find_7z()
    pspdecrypt = find_pspdecrypt()
    for inner, name in info["files"].items():
        encrypted = orig / Path(inner).name
        with encrypted.open("wb") as f:
            subprocess.run([seven_zip, "e", "-so", str(iso), inner],
                           stdout=f, stderr=subprocess.DEVNULL, check=True)
        out = orig / name
        out.unlink(missing_ok=True)
        subprocess.run([str(pspdecrypt), str(encrypted), "-o", str(out)], check=True,
                       stdout=subprocess.DEVNULL)
        if not out.exists() or out.read_bytes()[:4] != b"\x7fELF":
            sys.exit(f"{encrypted.name}: decryption failed")
        digest = sha1_of(out)
        want = expected.get(out.name)
        if want is None:
            print(f"{out.name}: extracted, SHA-1 {digest} (no reference)")
        elif digest != want:
            sys.exit(f"{out.name}: SHA-1 {digest}, expected {want}")
        else:
            print(f"{out.name}: OK")


if __name__ == "__main__":
    main()
