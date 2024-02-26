const std = @import("std");
const net = std.net;
const mem = std.mem;
const stdout = std.io.getStdOut().writer();
const heap_alloc = std.heap.page_allocator;

const BUFF_SIZE: u16 = 8192;
const MAX_PAGE_SIZE: u16 = 24;

const content_type = enum {
    HTML,
    IMAGE,
};

const method = enum {
    GET,
    HEAD,
};

const http_request = struct {
    method: method = method.GET,
    content_type: content_type = content_type.HTML,
    url: [MAX_PAGE_SIZE]u8,

    pub fn print(self: http_request) !void {
        try stdout.print("[{s}][{s}][{s}]\n", .{ self.method, self.content_type, self.url });
    }
};

fn parse_http(raw_buf: [BUFF_SIZE]u8) !http_request {
    var lines = mem.split(u8, &raw_buf, "\r\n");
    const first_line: []const u8 = lines.first();

    try stdout.print("space : {s}\n", .{first_line});

    var spaces = mem.split(u8, first_line, " ");
    const meth: []const u8 = spaces.first();
    const path: []const u8 = spaces.next().?;
    const vers: []const u8 = spaces.next().?;

    if (!mem.eql(u8, meth, "GET")) {
        return 1;
    }
    if (!mem.eql(u8, path, "/")) {
        return 1;
    }
    if (!mem.eql(u8, vers, "HTTP/1.1")) {
        return 1;
    }

    var rq = http_request{
        .url = path,
    };

    if (mem.eql(u8, path, "/image/")) {
        rq.content_type = content_type.IMAGE;
    }

    _ = try rq.print();

    return rq;
}

pub fn client_interract(client_stream: net.Stream) !u8 {
    // raw_buf for read
    var raw_buf: [BUFF_SIZE]u8 = undefined;

    _ = try client_stream.read(&raw_buf);
    const rq: http_request = try parse_http(raw_buf);
    _ = rq;

    const header = "HTTP/1.1 200 OK\r\n Date: Sun, 10 Dec 2077 12:13:37 CET\r\n Server: le_server/1.0 (lenux)\r\n Content-Type: text/html\r\n Connection: Closed\r\n\r\n <html><body><h1>Hello</h1></body></html>";

    _ = try client_stream.write(header);

    std.debug.print("request raw_buf is : {s}\n", .{raw_buf});
    return 1;
}

pub fn accept_client(server: *net.StreamServer) !net.Stream {
    const client = try server.accept();
    const client_addr = client.address;

    std.debug.print("client addr is : {any}\n", .{client_addr});

    return client.stream;
}

pub fn server_setup() !net.StreamServer {
    const addr = net.Address.initIp4(.{ 127, 0, 0, 1 }, 8080);
    var server = net.StreamServer.init(.{});
    server.reuse_address = true;
    _ = try server.listen(addr);

    std.debug.print("Server is listening on: {}\n", .{addr});

    return server;
}

// pub fn read_file(file_path: []const u8) !void {
//     var file = try std.fs.cwd().openFile(file_path, .{});
//     defer file.close();
//
//     var buf_reader = std.io.bufferedReader(file.reader());
//     var in_stream = buf_reader.reader();
//     _ = in_stream;
//     // in_stream.readBoundedBytes(comptime num_bytes: usize)
// }

// pub fn build_response() !void{
//     const file_path = try std.fmt.bufPrint("./files/ {s}", .{ world });
//
// }
