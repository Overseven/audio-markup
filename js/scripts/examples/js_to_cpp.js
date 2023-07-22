var a = 3 * 100;
debug_series(abs([-a*a, 44, -55, 66, -77]), "ser 1");
debug_series(abs([-2, 3, -4, -5, 6]), "ser 2");
debug_ranges([{start: 10, end: 15}, {start: 19, end: 25}, {start: 30, end: 45}], "rang 1");
return_ranges([{start: 1, end: 5}, {start: 9, end: 11}, {start: 16, end: 20}], "rang 2");
debug_value(123, "val11");
debug_value(1234, "val22");