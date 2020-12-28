#!/usr/bin/env rdmd
// SPDX-License-Identifier: ISC
import std.stdio;
import std.file;
import std.format;
import std.algorithm;
import std.bitmanip;
import std.exception;
import std.zlib;
import core.stdc.stdlib;

void create(string[] args)
{
    enforce(args.length > 1, "You must specify an output file.");
    enforce(args.length > 2, "You must specify at least one input file.");

    string outputPath = args[1];
    File output = File(outputPath, "wb");
    scope(failure) { outputPath.remove(); }

    string[] inputPaths = args[2..$];

    ulong[] fileSizes = new ulong[inputPaths.length];

    foreach (ulong i, string inputPath; inputPaths)
        fileSizes[i] = inputPath.getSize();

    // write the data directory
    ulong currentOffset = 0;
    for (ulong i = 0; i < inputPaths.length; ++i) {
        ulong fileSize = fileSizes[i];
        enforce(fileSize > 0, "The input file must not be empty.");
        enforce(cast(uint)fileSize == fileSize &&
                cast(uint)currentOffset == currentOffset,
                "The size of the input is too large.");
        output.writeBigEndian(cast(uint)fileSize);
        output.writeBigEndian(cast(uint)currentOffset);
        output.write(inputPaths[i]);
        ubyte[1] nullTerminator = [0];
        output.rawWrite(nullTerminator);
        currentOffset += fileSize;
    }
    output.writeBigEndian(0u);

    // write concatenated gzipped data
    Compress csr = new Compress(9, HeaderFormat.gzip);
    foreach (ulong i, string inputPath; inputPaths) {
        File input = File(inputPaths[i], "rb");
        ulong inputSize = 0;
        foreach (ubyte[] chunk; input.byChunk(8192)) {
            inputSize += chunk.length;
            output.rawWrite(csr.compress(chunk));
        }
        enforce(inputSize == fileSizes[i],
                "The file size read does not match with the data directory.");
    }
    output.rawWrite(csr.flush());
}

void contents(string[] args)
{
    enforce(args.length > 1, "You must specify an input file.");

    string inputPath = args[1];
    File input = File(inputPath, "rb");

    for (uint fileSize; (fileSize = readBigEndian!uint(input)) > 0; ) {
        ulong fileOffset = readBigEndian!uint(input);
        string fileName = input.readStringz();
        writefln(`file: "%s", size: %d, offset: %d`, fileName, fileSize, fileOffset);
    }
}

void main(string[] args)
{
    try {
        if (args.length < 2 || ["-h", "--help"].canFind(args[1])) {
            usage(stderr);
            return;
        }

        if (args[1] == "create")
            create(args[1..$]);
        else if (args[1] == "contents")
            contents(args[1..$]);
        else
            enforce(false, format("Invalid command: %s", args[1]));
    }
    catch (Exception e)
    {
        stderr.writeln(e.message);
        exit(1);
    }
}

void usage(File stream)
{
    stream.writeln(
"Usage:
    pak create <file.pak> <source-file...>
    pak contents <file.pak>");
}

void writeBigEndian(T)(File file, T value)
{
    ubyte[T.sizeof] buffer;
    ubyte[] range = buffer[0..$];
    range.write!(T, Endian.bigEndian)(value, 0);
    file.rawWrite(range);
}

T readBigEndian(T)(File file)
{
    ubyte[T.sizeof] buffer;
    enforce(file.rawRead(buffer).length == T.sizeof,
            "Not enough bytes for reading.");
    ubyte[] range = buffer[0..$];
    return range.read!(T, Endian.bigEndian)();
}

string readStringz(File file)
{
    char[] str;
    str.reserve(256);

    for (char c; file.rawRead((&c)[0..1]).length == 1; ) {
        if (c == '\0')
            return cast(immutable)str;
        str ~= c;
    }

    enforce("Not enough bytes for reading.");
    assert(0);
}
