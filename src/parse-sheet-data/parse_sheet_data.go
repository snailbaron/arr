package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"log"
	"maps"
	"os"
	"slices"
	"strconv"
	"strings"
)

type spriteSize struct {
	W int `json:"w"`
	H int `json:"h"`
}

func (ss spriteSize) String() string {
	return fmt.Sprintf("%dx%d", ss.W, ss.H)
}

type frame struct {
	X int `json:"x"`
	Y int `json:"y"`
	W int `json:"w"`
	H int `json:"h"`
}

func (f frame) String() string {
	return fmt.Sprintf("{x=%d y=%d w=%d h=%d}", f.X, f.Y, f.W, f.H)
}

type sheetFrameInfo struct {
	Filename         string     `json:"filename"`
	Frame            frame      `json:"frame"`
	Rotated          bool       `json:"rotated"`
	Trimmed          bool       `json:"trimmed"`
	SpriteSourceSize frame      `json:"spriteSourceSize"`
	SourceSize       spriteSize `json:"sourceSize"`
	Duration         int        `json:"duration"`
}

type sheet struct {
	Frames []sheetFrameInfo `json:"frames"`
}

func decodeSheet(input io.Reader) (*sheet, error) {
	decoder := json.NewDecoder(input)

	var sh sheet
	if err := decoder.Decode(&sh); err != nil {
		return nil, fmt.Errorf("failed to decode input JSON: %w", err)
	}
	return &sh, nil
}

type preparedSprite struct {
	size       spriteSize
	animations map[string][]frame
}

func prepareSheet(sh *sheet) (map[string]preparedSprite, error) {
	sprites := make(map[string]preparedSprite)

	for _, f := range sh.Frames {
		filenameFields := strings.Split(f.Filename, ":")
		if len(filenameFields) != 3 {
			return nil, fmt.Errorf(
				"filename %q must contain 3 colon-separated fields",
				f.Filename)
		}

		spriteName := filenameFields[0]
		animationName := filenameFields[1]
		frameIndex, err := strconv.Atoi(filenameFields[2])
		if err != nil {
			return nil, fmt.Errorf(
				"cannot parse filename %q: %q is not an integer",
				f.Filename, filenameFields[2])
		}

		if _, ok := sprites[spriteName]; !ok {
			sprites[spriteName] = preparedSprite{
				size:       f.SourceSize,
				animations: make(map[string][]frame),
			}
		}

		sprites[spriteName].animations[animationName] = insertAt(
			sprites[spriteName].animations[animationName],
			frameIndex,
			f.Frame)
	}

	return sprites, nil
}

func generateHeader(sprites map[string]preparedSprite, output io.Writer) error {
	p := NewErrPrinter(output)

	p.Print(`#pragma once

#include <array>
#include <span>

namespace assets {

constinit unsigned char sheet[]{
#embed <assets/sheet.png>
};

struct Frame {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
};

struct Size {
    int w = 0;
    int h = 0;
};

struct ButtonAnimations {
    std::span<Frame> neutral;
    std::span<Frame> focused;
    std::span<Frame> pressed;
    std::span<Frame> left;
};

struct ButtonSprite {
    Size size{};
    ButtonAnimations animations;
};

namespace sprites {

`)

	var allFrames []frame

	spriteNames := slices.Sorted(maps.Keys(sprites))
	for _, spriteName := range spriteNames {
		sprite := sprites[spriteName]

		spriteAnimationNames := slices.Sorted(maps.Keys(sprite.animations))
		isButtonSprite := slices.Equal(spriteAnimationNames,
			[]string{"focused", "left", "neutral", "pressed"})
		if !isButtonSprite {
			return fmt.Errorf(
				"only button sprites are supported now, got one with animations %v",
				spriteAnimationNames)
		}
		p.Printf("constinit ButtonSprite %s{\n", kebabToCamel(spriteName))
		p.Println("    .animations{")
		for _, spriteAnimationName := range []string{
			"neutral", "focused", "pressed", "left"} {

			animationFrames := sprite.animations[spriteAnimationName]

			framesBegin := len(allFrames)
			allFrames = append(allFrames, animationFrames...)

			p.Printf("        .%s{\n", spriteAnimationName)
			p.Printf("            std::span<Frame>(frames.data() + %d, %d\n",
				framesBegin, len(animationFrames))
			p.Println("        },")
		}
		p.Println("    },")
		p.Println("};")
	}

	p.Println("")
	p.Println("} // namespace sprites")
	p.Println("")

	p.Println("constinit auto frames = std::array{")
	for _, f := range allFrames {
		p.Printf("    Frame{.x = %d, .y = %d, .w = %d, .h = %d},\n",
			f.X, f.Y, f.W, f.H)
	}
	p.Println("};")

	p.Println("")
	p.Println("} // namespace assets")

	if err := p.Err(); err != nil {
		return fmt.Errorf("failed to generate header: %w", err)
	}
	return nil
}

func main() {
	inputPath := flag.String("input", "-", "input sheet JSON file")
	outputPath := flag.String("output", "-", "output header file")
	flag.Parse()

	if len(flag.Args()) > 0 {
		flag.Usage()
		os.Exit(2)
	}

	input, err := optionalInputFile(*inputPath)
	if err != nil {
		log.Fatal(err)
	}
	output, err := optionalOutputFile(*outputPath)
	if err != nil {
		log.Fatal(err)
	}

	sh, err := decodeSheet(input)
	if err != nil {
		log.Fatalf("cannot decode sheet: %v", err)
	}

	sprites, err := prepareSheet(sh)
	if err != nil {
		log.Fatalf("cannot prepare sheet: %v", err)
	}

	generateHeader(sprites, output)
}
