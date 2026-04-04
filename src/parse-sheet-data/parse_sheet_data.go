package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"maps"
	"os"
	"slices"
	"strconv"
	"strings"

	"aseprite"
	"gogo/moreio"
	"gogo/xs"
)

type frameRange struct {
	begin int
	size  int
}

type sprite struct {
	size       aseprite.Size
	animations map[string]frameRange
}

type spriteData struct {
	frames  []aseprite.Frame
	sprites map[string]sprite
}

var buttonSpriteAnimationNames = []string{"neutral", "focused", "pressed", "left"}

func prepareSpriteData(sh *aseprite.Sheet) (*spriteData, error) {
	spriteSizes := make(map[string]aseprite.Size)
	frames := make(map[string]map[string][]aseprite.Frame)

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

		if _, ok := frames[spriteName]; !ok {
			frames[spriteName] = make(map[string][]aseprite.Frame)
		}
		spriteSizes[spriteName] = f.SourceSize
		frames[spriteName][animationName] = xs.InsertAt(
			frames[spriteName][animationName],
			frameIndex,
			f.Frame)
	}

	spriteData := spriteData{sprites: make(map[string]sprite)}

	for spriteName, spriteFrames := range frames {
		spriteData.sprites[spriteName] = sprite{
			size:       spriteSizes[spriteName],
			animations: make(map[string]frameRange),
		}

		for animationName, animationFrames := range spriteFrames {
			spriteData.sprites[spriteName].animations[animationName] = frameRange{
				begin: len(spriteData.frames), size: len(animationFrames),
			}
			spriteData.frames = append(spriteData.frames, animationFrames...)
		}
	}

	return &spriteData, nil
}

func generateHeader(sd *spriteData, output io.Writer) error {
	p := moreio.NewErrPrinter(output)

	p.Print(`#pragma once

#include "sdlxx.hpp"

#include <array>
#include <span>

namespace assets {

namespace internal {

#include "src/assets/sheet.hpp"

} // namespace internal

inline sdl::Texture sheetTexture;

constexpr void init(sdl::Renderer& rr)
{
    sheetTexture = rr.loadTexture(internal::bytes);
}

struct Size {
    float w = 0.f;
    float h = 0.f;
};

struct ButtonAnimations {
    std::span<const SDL_FRect> neutral;
    std::span<const SDL_FRect> focused;
    std::span<const SDL_FRect> pressed;
    std::span<const SDL_FRect> left;
};

struct ButtonSprite {
    static sdl::Texture& texture()
    {
        return sheetTexture;
    }

    Size size;
    ButtonAnimations animations;
};

`)

	p.Println("constinit inline const auto frames = std::array{")
	for _, f := range sd.frames {
		p.Printf("    SDL_FRect{.x = %d, .y = %d, .w = %d, .h = %d},\n",
			f.X, f.Y, f.W, f.H)
	}
	p.Println("};")
	p.Println("")

	p.Println("namespace sprites {")
	p.Println("")

	for spriteName, spr := range xs.InOrder(sd.sprites) {
		p.Printf("constinit inline const ButtonSprite %s{\n", kebabToCamel(spriteName))
		p.Printf("    .size = Size{.w = %d, .h = %d},\n", spr.size.W, spr.size.H)
		p.Printf("    .animations{\n")

		if !xs.SameElements(
			slices.Collect(maps.Keys(spr.animations)),
			buttonSpriteAnimationNames) {

			return fmt.Errorf(
				"only button sprites are supported (animations %v)",
				buttonSpriteAnimationNames)
		}

		for _, animationName := range buttonSpriteAnimationNames {
			animationFrames := spr.animations[animationName]
			p.Printf("        .%s = std::span<const SDL_FRect>(frames.data() + %d, %d),\n",
				kebabToCamel(animationName),
				animationFrames.begin,
				animationFrames.size)
		}
		p.Printf("    },\n")
		p.Printf("};\n")
	}

	p.Println("")
	p.Println("} // namespace sprites")
	p.Println("")
	p.Println("} // namespace assets")

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

	input, err := moreio.OptionalInputFile(*inputPath)
	if err != nil {
		log.Fatal(err)
	}
	output, err := moreio.OptionalOutputFile(*outputPath)
	if err != nil {
		log.Fatal(err)
	}

	sh, err := aseprite.DecodeAsepriteSheet(input)
	if err != nil {
		log.Fatalf("cannot decode sheet: %v", err)
	}

	sd, err := prepareSpriteData(sh)
	if err != nil {
		log.Fatalf("cannot prepare sheet: %v", err)
	}

	generateHeader(sd, output)
}
