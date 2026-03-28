package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"log"

	"gogo/moreio"
)

func binToHeader(in io.Reader, out io.Writer, namespace string) error {
	w := bufio.NewWriter(out)
	p := moreio.NewErrPrinter(w)

	p.Printf(`#pragma once

#include <array>
#include <cstddef>

namespace %s {

namespace literals {

consteval std::byte operator""_b(unsigned long long b)
{
    return std::byte{static_cast<unsigned char>(b)};
}

} // namespace literals

using namespace literals;

constinit const auto bytes = std::array{
`, namespace)

	scanner := bufio.NewScanner(in)
	scanner.Split(bufio.ScanBytes)
	lineLength := 3
	p.Print("   ")
	for scanner.Scan() {
		b := scanner.Bytes()[0]
		s := fmt.Sprintf(" 0x%02x_b,", b)
		if lineLength+len(s) > 80 {
			p.Printf("\n   ")
			lineLength = 3
		}
		lineLength += len(s)
		p.Print(s)
	}
	p.Printf(`
};

} // namespace %s`, namespace)

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
	namespace := flag.String("namespace", "data", "namespace name")
	flag.Parse()

	in, err := moreio.OptionalInputFile(*inputPath)
	if err != nil {
		log.Fatal(err)
	}
	out, err := moreio.OptionalOutputFile(*outputPath)
	if err != nil {
		log.Fatal(err)
	}

	if err := binToHeader(in, out, *namespace); err != nil {
		log.Fatal(err)
	}
}
