package moreio

import (
	"io"
	"os"
)

func OptionalInputFile(inputFilePathOrDash string) (io.ReadCloser, error) {
	if inputFilePathOrDash == "-" {
		return io.NopCloser(os.Stdin), nil
	}
	return os.Open(inputFilePathOrDash)
}

type NopWriteCloser struct {
	io.Writer
}

func (NopWriteCloser) Close() error {
	return nil
}

func OptionalOutputFile(outputFilePathOrDash string) (io.WriteCloser, error) {
	if outputFilePathOrDash == "-" {
		return NopWriteCloser{os.Stdout}, nil
	}
	return os.Create(outputFilePathOrDash)
}
