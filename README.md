# audio-markup

![audio-markup](https://user-images.githubusercontent.com/43533945/236845295-7ffda4d9-8a92-4014-b083-5bf9443202e4.PNG)

## Scripts
### Examples
#### Using system function on selected sample file
```js
// get samples from selected audio file
var input_samples = get_input_samples();
// process samples
var processed = abs(input_samples);
// return processed samples with "processed_data" label by using system function "view_series"
view_series(processed, "processed_data");
```

#### Return range data
```js
// make simple ranges data
var range1 = {start: 10, end: 15};
var range2 = {start: 20, end: 30};
// return array of ranges with "my_ranges" label by using system function "view_ranges"
view_ranges([range1, range2], "my_ranges");
```

#### Return a value
```js
// make simple number value
var value = 77;
// return a value with "my_debug_value" label by using system function "debug_value"
debug_value(value, "my_debug_value");
```

#### Return multiple series, ranges and values
```js
view_series(abs([-a*a, 44, -55, 66, -77]), "series_1");
view_series(abs([-2, 3, -4, -5, 6]), "series_1");

view_ranges([{start: 10, end: 15}, {start: 19, end: 25}, {start: 30, end: 45}], "ranges_1");
view_ranges([{start: 1, end: 5}, {start: 9, end: 11}, {start: 16, end: 20}], "ranges_2");

debug_value(123, "my_debug_value_1");
debug_value(1234, "my_debug_value_1");
```
