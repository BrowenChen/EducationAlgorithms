# Stylus Type Utils

Much needed Stylus typeography unit coersion and normalization. Provides several common unit conversion functions to convert between types effortlessly and in sync with the content font size.

## Installation

    $ npm install stylus-type-utils --save

## API

To use the `stylus-type-utils` library within your framework, use `@import` or use via the Stylus JS API.

    base-font-size = 100% // Change to adjust the conversion process
    support-for-ie = true // Set to `false` for no `px` output with `rem`

    px(unit, ignore-base-font-size)
    pt(unit, ignore-base-font-size)
    em(unit, ignore-base-font-size)
    rem(unit, ignore-base-font-size)
    percent(unit, ignore-base-font-size)

When using the functions, omitting a unit type will assume the function unit type. E.g. `px(16) -> px(16px)`.

The second parameter, `ignore-base-font-size`, allows the function to act as if the `base-font-size` was to set `100%`.

## Example Usage

    @import "type-utils"

    base-font-size = 75%

    px(16) // 12px
    px(16, true) // 16px
    px(16px) // 12px
    px(1.2em) // 14.4px
    em(16px) // 0.75em

    rem(16px) // 12px, 0.75rem

    support-for-ie = false // No more px + rem output when using rem function

    rem(16px) // 0.75rem
    rem(16px, true) // 1rem

    base-font-size = 100%

    rem(16px) // 1rem
   

## License

MIT
