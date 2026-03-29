package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"log"

	"gogo/moreio"
)

func binToHeader(in io.Reader, out io.Writer) error {
	w := bufio.NewWriter(out)
	p := moreio.NewErrPrinter(w)

	p.Print(`#pragma once

#include <array>
#include <cstddef>
#include <span>

namespace internal {

consteval std::byte operator""_b(unsigned long long b)
{
    return std::byte{static_cast<unsigned char>(b)};
}

constinit inline const auto zeroTerminatedBytes = std::array{
`)

	scanner := bufio.NewScanner(in)
	scanner.Split(bufio.ScanBytes)
	lineLength := 3
	p.Print("   ")

	addByte := func(b byte) {
		s := fmt.Sprintf(" 0x%02x_b,", b)
		if lineLength+len(s) > 80 {
			p.Printf("\n   ")
			lineLength = 3
		}
		lineLength += len(s)
		p.Print(s)
	}

	for scanner.Scan() {
		b := scanner.Bytes()[0]
		addByte(b)
	}
	addByte(0)
	p.Print(`
};

} // namespace internal

using namespace internal;

constinit inline const auto bytes = std::span<const std::byte>(
    internal::zeroTerminatedBytes.data(),
    internal::zeroTerminatedBytes.size() - 1);
`)

	if err := scanner.Err(); err != nil {
		return fmt.Errorf("failed to read input: %w", err)
	}
	if err := p.Err(); err != nil {
		return fmt.Errorf("failed to write: %w", err)
	}
	if err := w.Flush(); err != nil {
		return fmt.Errorf("failed to write: %w", err)
	}

	return nil
}

func main() {
	inputPath := flag.String("input", "-", "input binary file to encode")
	outputPath := flag.String("output", "-", "header to generate")
	flag.Parse()

	in, err := moreio.OptionalInputFile(*inputPath)
	if err != nil {
		log.Fatal(err)
	}
	out, err := moreio.OptionalOutputFile(*outputPath)
	if err != nil {
		log.Fatal(err)
	}

	if err := binToHeader(in, out); err != nil {
		log.Fatal(err)
	}
}
