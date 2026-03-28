package main

import (
	"fmt"
	"io"
	"os"
	"unicode"
)

func insertAt[S ~[]E, E any](s S, i int, value E) []E {
	if i >= cap(s) {
		s = append(s[:cap(s)], make([]E, i+1-cap(s))...)[:i+1]
	} else if i >= len(s) {
		s = s[:i+1]
	}
	s[i] = value
	return s
}

func kebabToCamel(s string) string {
	result := ""

	convert := false
	for _, c := range s {
		if c == '-' {
			convert = true
			continue
		}

		if convert {
			convert = false
			c = unicode.ToUpper(c)
		}
		result += string(c)
	}

	return result
}

func optionalInputFile(inputFilePathOrDash string) (io.ReadCloser, error) {
	if inputFilePathOrDash == "-" {
		return io.NopCloser(os.Stdin), nil
	}
	return os.Open(inputFilePathOrDash)
}

type nopWriteCloser struct {
	io.Writer
}

func (nopWriteCloser) Close() error {
	return nil
}

func optionalOutputFile(outputFilePathOrDash string) (io.WriteCloser, error) {
	if outputFilePathOrDash == "-" {
		return nopWriteCloser{os.Stdout}, nil
	}
	return os.Create(outputFilePathOrDash)
}

type ErrPrinter struct {
	out io.Writer
	err error
}

func NewErrPrinter(w io.Writer) *ErrPrinter {
	return &ErrPrinter{out: w}
}

func (w *ErrPrinter) Print(a ...any) {
	if w.err == nil {
		_, w.err = fmt.Fprint(w.out, a...)
	}
}

func (w *ErrPrinter) Printf(format string, a ...any) {
	if w.err == nil {
		_, w.err = fmt.Fprintf(w.out, format, a...)
	}
}

func (w *ErrPrinter) Println(a ...any) {
	if w.err == nil {
		_, w.err = fmt.Fprintln(w.out, a...)
	}
}

func (w *ErrPrinter) Err() error {
	return w.err
}
