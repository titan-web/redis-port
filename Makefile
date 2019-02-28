.DEFAULT_GOAL := build-all

export GO15VENDOREXPERIMENT=1

UNAME_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

GO_BUILD := go build
GO_TEST  := go test

ifeq ($(UNAME_S),Linux)
GO_BUILD += -tags "use_jemalloc"
GO_TEST  += -tags "use_jemalloc"
build-deps: build-jemalloc
endif

ifeq ($(UNAME_S),Darwin)
GO_BUILD += -ldflags="-s"
GO_TEST  += -ldflags="-s"
endif

build-all: redis-sync redis-dump redis-decode redis-restore

GO_SRCS := $(shell bash -c 'echo cmd/{version,flags,libs,iolibs}.go')

build-deps:
	@mkdir -p bin && bash version

redis-sync: build-deps
	${GO_BUILD} -o bin/$@ \
		${GO_SRCS} cmd/sync.go

redis-dump: build-deps
	${GO_BUILD} -o bin/$@ \
		${GO_SRCS} cmd/dump.go

redis-decode: build-deps
	${GO_BUILD} -o bin/$@ \
		${GO_SRCS} cmd/decode.go

redis-restore: build-deps
	${GO_BUILD} -o bin/$@ \
		${GO_SRCS} cmd/restore.go

clean:
	@rm -rf bin

distclean: clean
	@make distclean --no-print-directory --quiet -C third_party/redis
	@[ ! -f third_party/jemalloc/Makefile ] || \
		make distclean --no-print-directory --quiet -C third_party/jemalloc

gotest: build-deps gotest-flags
	${GO_TEST} -v ./pkg/...

gotest-flags: build-deps
	${GO_TEST} -v ./cmd/version.go ./cmd/flags.go ./cmd/flags_test.go

jemalloc:
	@cd third_party/jemalloc && \
		./autogen.sh --with-jemalloc-prefix="je_" && make -j

build-jemalloc:
	@[ -f third_party/jemalloc/lib/libjemalloc_pic.a ] || \
		make jemalloc --no-print-directory
