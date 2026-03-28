package moreio

import (
	"fmt"
	"io"
)

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
