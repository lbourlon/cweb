const std = @import("std");
const slayer = @import("slayer.zig");
const net = std.net;

pub fn main() !void {
    var server = try slayer.server_setup();
    defer server.deinit();

    while (true) {
        var client_stream = try slayer.accept_client(&server);
        defer client_stream.close();
        _ = try slayer.client_interract(client_stream);
    }
}
