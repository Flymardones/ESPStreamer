idf_component_register(
        SRCS
        "mad-mrbuddycasino/align.c"
        "mad-mrbuddycasino/bit.c"
        "mad-mrbuddycasino/decoder.c"
        "mad-mrbuddycasino/fixed.c"
        "mad-mrbuddycasino/frame.c"
        "mad-mrbuddycasino/huffman.c"
        "mad-mrbuddycasino/layer3.c"
        "mad-mrbuddycasino/layer12.c"
        "mad-mrbuddycasino/stream.c"
        "mad-mrbuddycasino/synth_stereo.c"
        "mad-mrbuddycasino/timer.c"
        "mad-mrbuddycasino/version.c"

        INCLUDE_DIRS
        "mad-mrbuddycasino"

        REQUIRES AudioDecoding
)

target_compile_options(${COMPONENT_TARGET} PUBLIC -O2 -w) #Mad makes tons of errors and warnings, so we disable them instead of fixing, for now..
