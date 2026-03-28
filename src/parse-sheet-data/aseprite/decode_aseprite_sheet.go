package aseprite

import (
	"encoding/json"
	"fmt"
	"io"
)

type Size struct {
	W int `json:"w"`
	H int `json:"h"`
}

func (ss Size) String() string {
	return fmt.Sprintf("%dx%d", ss.W, ss.H)
}

type Frame struct {
	X int `json:"x"`
	Y int `json:"y"`
	W int `json:"w"`
	H int `json:"h"`
}

func (f Frame) String() string {
	return fmt.Sprintf("{x=%d y=%d w=%d h=%d}", f.X, f.Y, f.W, f.H)
}

type FrameInfo struct {
	Filename         string `json:"filename"`
	Frame            Frame  `json:"frame"`
	Rotated          bool   `json:"rotated"`
	Trimmed          bool   `json:"trimmed"`
	SpriteSourceSize Frame  `json:"spriteSourceSize"`
	SourceSize       Size   `json:"sourceSize"`
	Duration         int    `json:"duration"`
}

type Sheet struct {
	Frames []FrameInfo `json:"frames"`
}

func DecodeAsepriteSheet(input io.Reader) (*Sheet, error) {
	decoder := json.NewDecoder(input)

	var sh Sheet
	if err := decoder.Decode(&sh); err != nil {
		return nil, fmt.Errorf("failed to decode input JSON: %w", err)
	}
	return &sh, nil
}
