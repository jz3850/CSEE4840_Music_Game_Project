/*
 * Avalon memory-mapped peripheral that generates VGA
 *
 * Stephen A. Edwards
 * Columbia University
 */

module vga_framebuffer (
    input  logic         clk,
    input  logic         reset,
    input  logic [7:0]   writedata,    // 8-bit color index
    input  logic         write,
    input  logic         chipselect,
    input  logic [18:0]  address,      // 19-bit pixel address

    output logic [7:0]   VGA_R,
    output logic [7:0]   VGA_G,
    output logic [7:0]   VGA_B,
    output logic         VGA_CLK,
    output logic         VGA_HS,
    output logic         VGA_VS,
    output logic         VGA_BLANK_n,
    output logic         VGA_SYNC_n
);

  logic [10:0] hcount;
  logic [ 9:0] vcount;
  logic [ 8:0] pixel_y;
  logic [ 9:0] pixel_x;
  logic [18:0] read_addr;
  logic [18:0] read_addr_reg;
  logic [18:0] write_addr;

  logic [7:0]  pixel_data;
  logic [7:0]  write_data;
  logic        write_mem;

  assign read_addr = read_addr_reg;

  vga_counters counters (
      .clk50(clk),
      .*
  );

  vga_mem mem (
      .clk(clk),
      .ra(read_addr),
      .wa(write_addr),
      .write(write_mem),
      .wd(write_data),
      .rd(pixel_data)
  );

  // Write logic: on Avalon write, capture address and data
  always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
      write_addr <= 19'd0;
      write_data <= 8'd0;
      write_mem  <= 1'b0;
    end else begin
      write_mem <= 1'b0;
      if (chipselect && write) begin
        write_addr <= address;
        write_data <= writedata;
        write_mem  <= 1'b1;
      end
    end
  end 

  
always_ff @(posedge clk or posedge reset) begin
  if (reset) begin
    // pixel_x <= 10'd0;
    // pixel_y <= 9'd0;
    read_addr_reg <= 19'd0;
  end else begin
    if (VGA_BLANK_n) begin
      if ((hcount[10:1] < 10'd640) && (vcount < 10'd480)) begin
        // pixel_x <= hcount[10:1];
        // pixel_y <= vcount[8:0];
        read_addr_reg <= vcount[8:0] * 640 + hcount[10:1];
      end
    end
  end
end

  // Pixel color mapping: mock placeholder
  always_comb begin
    {VGA_R, VGA_G, VGA_B} = {8'h0, 8'h0, 8'h0};
    if (VGA_BLANK_n) begin
      case (pixel_data)
        8'd0: {VGA_R, VGA_G, VGA_B} = 24'h000000;
        8'd1: {VGA_R, VGA_G, VGA_B} = 24'hffffff;
        8'd2: {VGA_R, VGA_G, VGA_B} = 24'hffff00;
        8'd3: {VGA_R, VGA_G, VGA_B} = 24'h9e4f71;
        8'd4: {VGA_R, VGA_G, VGA_B} = 24'hd66f34;
        8'd5: {VGA_R, VGA_G, VGA_B} = 24'hf755aa;
        8'd6: {VGA_R, VGA_G, VGA_B} = 24'he5790d;
        8'd7: {VGA_R, VGA_G, VGA_B} = 24'h5d695a;
        8'd8: {VGA_R, VGA_G, VGA_B} = 24'hfcfcf9;
        8'd9: {VGA_R, VGA_G, VGA_B} = 24'hb0a392;
        8'd10: {VGA_R, VGA_G, VGA_B} = 24'h8f2655;
        8'd11: {VGA_R, VGA_G, VGA_B} = 24'hcc9ba0;
        8'd12: {VGA_R, VGA_G, VGA_B} = 24'hc5e773;
        8'd13: {VGA_R, VGA_G, VGA_B} = 24'h9f9d9c;
        8'd14: {VGA_R, VGA_G, VGA_B} = 24'hedeceb;
        8'd15: {VGA_R, VGA_G, VGA_B} = 24'ha15524;
        8'd16: {VGA_R, VGA_G, VGA_B} = 24'h27d1cd;
        8'd17: {VGA_R, VGA_G, VGA_B} = 24'h657751;
        8'd18: {VGA_R, VGA_G, VGA_B} = 24'h53a058;
        8'd19: {VGA_R, VGA_G, VGA_B} = 24'hb6b3a9;
        8'd20: {VGA_R, VGA_G, VGA_B} = 24'he0c76b;
        8'd21: {VGA_R, VGA_G, VGA_B} = 24'hc1bcbb;
        8'd22: {VGA_R, VGA_G, VGA_B} = 24'hfb6dcd;
        8'd23: {VGA_R, VGA_G, VGA_B} = 24'h9c6b93;
        8'd24: {VGA_R, VGA_G, VGA_B} = 24'h9ca999;
        8'd25: {VGA_R, VGA_G, VGA_B} = 24'habcf99;
        8'd26: {VGA_R, VGA_G, VGA_B} = 24'h22b8c2;
        8'd27: {VGA_R, VGA_G, VGA_B} = 24'ha5a2a0;
        8'd28: {VGA_R, VGA_G, VGA_B} = 24'ha69260;
        8'd29: {VGA_R, VGA_G, VGA_B} = 24'h787c81;
        8'd30: {VGA_R, VGA_G, VGA_B} = 24'h76c5c5;
        8'd31: {VGA_R, VGA_G, VGA_B} = 24'he09e6c;
        8'd32: {VGA_R, VGA_G, VGA_B} = 24'hf8c58c;
        8'd33: {VGA_R, VGA_G, VGA_B} = 24'h84ae76;
        8'd34: {VGA_R, VGA_G, VGA_B} = 24'hb57954;
        8'd35: {VGA_R, VGA_G, VGA_B} = 24'h666254;
        8'd36: {VGA_R, VGA_G, VGA_B} = 24'hdfb68b;
        8'd37: {VGA_R, VGA_G, VGA_B} = 24'h148968;
        8'd38: {VGA_R, VGA_G, VGA_B} = 24'h55c561;
        8'd39: {VGA_R, VGA_G, VGA_B} = 24'hddd8cc;
        8'd40: {VGA_R, VGA_G, VGA_B} = 24'h52b726;
        8'd41: {VGA_R, VGA_G, VGA_B} = 24'h256f76;
        8'd42: {VGA_R, VGA_G, VGA_B} = 24'hd39772;
        8'd43: {VGA_R, VGA_G, VGA_B} = 24'hbf9306;
        8'd44: {VGA_R, VGA_G, VGA_B} = 24'h705926;
        8'd45: {VGA_R, VGA_G, VGA_B} = 24'h656362;
        8'd46: {VGA_R, VGA_G, VGA_B} = 24'h6c2d15;
        8'd47: {VGA_R, VGA_G, VGA_B} = 24'h847978;
        8'd48: {VGA_R, VGA_G, VGA_B} = 24'hb38870;
        8'd49: {VGA_R, VGA_G, VGA_B} = 24'h91722f;
        8'd50: {VGA_R, VGA_G, VGA_B} = 24'h226013;
        8'd51: {VGA_R, VGA_G, VGA_B} = 24'h727271;
        8'd52: {VGA_R, VGA_G, VGA_B} = 24'he8902a;
        8'd53: {VGA_R, VGA_G, VGA_B} = 24'h778d5f;
        8'd54: {VGA_R, VGA_G, VGA_B} = 24'h9a690e;
        8'd55: {VGA_R, VGA_G, VGA_B} = 24'hd5c4bb;
        8'd56: {VGA_R, VGA_G, VGA_B} = 24'h3a9f54;
        8'd57: {VGA_R, VGA_G, VGA_B} = 24'h7a4c1d;
        8'd58: {VGA_R, VGA_G, VGA_B} = 24'h17a294;
        8'd59: {VGA_R, VGA_G, VGA_B} = 24'he3e2d8;
        8'd60: {VGA_R, VGA_G, VGA_B} = 24'h8a6936;
        8'd61: {VGA_R, VGA_G, VGA_B} = 24'he48f07;
        8'd62: {VGA_R, VGA_G, VGA_B} = 24'h602b09;
        8'd63: {VGA_R, VGA_G, VGA_B} = 24'h6ca158;
        8'd64: {VGA_R, VGA_G, VGA_B} = 24'h6f5f4e;
        8'd65: {VGA_R, VGA_G, VGA_B} = 24'h9fbdc2;
        8'd66: {VGA_R, VGA_G, VGA_B} = 24'h996a5c;
        8'd67: {VGA_R, VGA_G, VGA_B} = 24'hf83ec8;
        8'd68: {VGA_R, VGA_G, VGA_B} = 24'h919192;
        8'd69: {VGA_R, VGA_G, VGA_B} = 24'hc4b5a0;
        8'd70: {VGA_R, VGA_G, VGA_B} = 24'hd8ad26;
        8'd71: {VGA_R, VGA_G, VGA_B} = 24'hf0a670;
        8'd72: {VGA_R, VGA_G, VGA_B} = 24'h754d27;
        8'd73: {VGA_R, VGA_G, VGA_B} = 24'hf6ca70;
        8'd74: {VGA_R, VGA_G, VGA_B} = 24'hecebea;
        8'd75: {VGA_R, VGA_G, VGA_B} = 24'hf8a012;
        8'd76: {VGA_R, VGA_G, VGA_B} = 24'h1e7a83;
        8'd77: {VGA_R, VGA_G, VGA_B} = 24'h3a444b;
        8'd78: {VGA_R, VGA_G, VGA_B} = 24'hb5673a;
        8'd79: {VGA_R, VGA_G, VGA_B} = 24'hedefed;
        8'd80: {VGA_R, VGA_G, VGA_B} = 24'hbec0c0;
        8'd81: {VGA_R, VGA_G, VGA_B} = 24'h7b726a;
        8'd82: {VGA_R, VGA_G, VGA_B} = 24'h8b7550;
        8'd83: {VGA_R, VGA_G, VGA_B} = 24'h463a35;
        8'd84: {VGA_R, VGA_G, VGA_B} = 24'h6d2d15;
        8'd85: {VGA_R, VGA_G, VGA_B} = 24'h64c52f;
        8'd86: {VGA_R, VGA_G, VGA_B} = 24'h45403c;
        8'd87: {VGA_R, VGA_G, VGA_B} = 24'h7d8185;
        8'd88: {VGA_R, VGA_G, VGA_B} = 24'hcf950f;
        8'd89: {VGA_R, VGA_G, VGA_B} = 24'hefab60;
        8'd90: {VGA_R, VGA_G, VGA_B} = 24'h656058;
        8'd91: {VGA_R, VGA_G, VGA_B} = 24'h5c9fa0;
        8'd92: {VGA_R, VGA_G, VGA_B} = 24'he6a310;
        8'd93: {VGA_R, VGA_G, VGA_B} = 24'hb99b89;
        8'd94: {VGA_R, VGA_G, VGA_B} = 24'hfdfdfd;
        8'd95: {VGA_R, VGA_G, VGA_B} = 24'h82bc2e;
        8'd96: {VGA_R, VGA_G, VGA_B} = 24'hf1d7ab;
        8'd97: {VGA_R, VGA_G, VGA_B} = 24'h67a41f;
        8'd98: {VGA_R, VGA_G, VGA_B} = 24'h473e40;
        8'd99: {VGA_R, VGA_G, VGA_B} = 24'h9ed35a;
        8'd100: {VGA_R, VGA_G, VGA_B} = 24'hfdd130;
        8'd101: {VGA_R, VGA_G, VGA_B} = 24'hdfd6cf;
        8'd102: {VGA_R, VGA_G, VGA_B} = 24'hf7338e;
        8'd103: {VGA_R, VGA_G, VGA_B} = 24'h92d227;
        8'd104: {VGA_R, VGA_G, VGA_B} = 24'he4cf9d;
        8'd105: {VGA_R, VGA_G, VGA_B} = 24'h923901;
        8'd106: {VGA_R, VGA_G, VGA_B} = 24'hab8770;
        8'd107: {VGA_R, VGA_G, VGA_B} = 24'hb6907a;
        8'd108: {VGA_R, VGA_G, VGA_B} = 24'h99d2a3;
        8'd109: {VGA_R, VGA_G, VGA_B} = 24'hebdaa2;
        8'd110: {VGA_R, VGA_G, VGA_B} = 24'h595a5c;
        8'd111: {VGA_R, VGA_G, VGA_B} = 24'heae0a2;
        8'd112: {VGA_R, VGA_G, VGA_B} = 24'heee8e8;
        8'd113: {VGA_R, VGA_G, VGA_B} = 24'hd86492;
        8'd114: {VGA_R, VGA_G, VGA_B} = 24'hd499b4;
        8'd115: {VGA_R, VGA_G, VGA_B} = 24'hb29992;
        8'd116: {VGA_R, VGA_G, VGA_B} = 24'hede8ec;
        8'd117: {VGA_R, VGA_G, VGA_B} = 24'h18a293;
        8'd118: {VGA_R, VGA_G, VGA_B} = 24'h1dc3bc;
        8'd119: {VGA_R, VGA_G, VGA_B} = 24'h91286a;
        8'd120: {VGA_R, VGA_G, VGA_B} = 24'hae6f8a;
        8'd121: {VGA_R, VGA_G, VGA_B} = 24'ha19a9b;
        8'd122: {VGA_R, VGA_G, VGA_B} = 24'h2e7154;
        8'd123: {VGA_R, VGA_G, VGA_B} = 24'hac8a2c;
        8'd124: {VGA_R, VGA_G, VGA_B} = 24'h91612a;
        8'd125: {VGA_R, VGA_G, VGA_B} = 24'hce7947;
        8'd126: {VGA_R, VGA_G, VGA_B} = 24'hdecf77;
        8'd127: {VGA_R, VGA_G, VGA_B} = 24'h88817d;
        8'd128: {VGA_R, VGA_G, VGA_B} = 24'he5d49b;
        8'd129: {VGA_R, VGA_G, VGA_B} = 24'h0f0c0c;
        8'd130: {VGA_R, VGA_G, VGA_B} = 24'h4d7c80;
        8'd131: {VGA_R, VGA_G, VGA_B} = 24'hcbae65;
        8'd132: {VGA_R, VGA_G, VGA_B} = 24'hd6cc9d;
        8'd133: {VGA_R, VGA_G, VGA_B} = 24'he7a410;
        8'd134: {VGA_R, VGA_G, VGA_B} = 24'hc8b795;
        8'd135: {VGA_R, VGA_G, VGA_B} = 24'h6d6a5c;
        8'd136: {VGA_R, VGA_G, VGA_B} = 24'hb2ca87;
        8'd137: {VGA_R, VGA_G, VGA_B} = 24'hfefefe;
        8'd138: {VGA_R, VGA_G, VGA_B} = 24'he57e45;
        8'd139: {VGA_R, VGA_G, VGA_B} = 24'h369236;
        8'd140: {VGA_R, VGA_G, VGA_B} = 24'hd8d4d4;
        8'd141: {VGA_R, VGA_G, VGA_B} = 24'h333c43;
        8'd142: {VGA_R, VGA_G, VGA_B} = 24'h686768;
        8'd143: {VGA_R, VGA_G, VGA_B} = 24'h7a5d4a;
        8'd144: {VGA_R, VGA_G, VGA_B} = 24'ha7d0cf;
        8'd145: {VGA_R, VGA_G, VGA_B} = 24'h957251;
        8'd146: {VGA_R, VGA_G, VGA_B} = 24'h9d6a0d;
        8'd147: {VGA_R, VGA_G, VGA_B} = 24'h686b68;
        8'd148: {VGA_R, VGA_G, VGA_B} = 24'h79cc9d;
        8'd149: {VGA_R, VGA_G, VGA_B} = 24'hf5f2e9;
        8'd150: {VGA_R, VGA_G, VGA_B} = 24'hb78b74;
        8'd151: {VGA_R, VGA_G, VGA_B} = 24'hfbfbf9;
        8'd152: {VGA_R, VGA_G, VGA_B} = 24'hfdcc16;
        8'd153: {VGA_R, VGA_G, VGA_B} = 24'hfac76d;
        8'd154: {VGA_R, VGA_G, VGA_B} = 24'hc9a536;
        8'd155: {VGA_R, VGA_G, VGA_B} = 24'heac9a7;
        8'd156: {VGA_R, VGA_G, VGA_B} = 24'h23631d;
        8'd157: {VGA_R, VGA_G, VGA_B} = 24'hd6b197;
        8'd158: {VGA_R, VGA_G, VGA_B} = 24'ha87150;
        8'd159: {VGA_R, VGA_G, VGA_B} = 24'hb2872f;
        8'd160: {VGA_R, VGA_G, VGA_B} = 24'hffffec;
        8'd161: {VGA_R, VGA_G, VGA_B} = 24'hc1c0be;
        8'd162: {VGA_R, VGA_G, VGA_B} = 24'he3e7d8;
        8'd163: {VGA_R, VGA_G, VGA_B} = 24'h81033a;
        8'd164: {VGA_R, VGA_G, VGA_B} = 24'ha47354;
        8'd165: {VGA_R, VGA_G, VGA_B} = 24'he8eae8;
        8'd166: {VGA_R, VGA_G, VGA_B} = 24'h060805;
        8'd167: {VGA_R, VGA_G, VGA_B} = 24'h90b35f;
        8'd168: {VGA_R, VGA_G, VGA_B} = 24'haf968b;
        8'd169: {VGA_R, VGA_G, VGA_B} = 24'hf47c03;
        8'd170: {VGA_R, VGA_G, VGA_B} = 24'h706971;
        8'd171: {VGA_R, VGA_G, VGA_B} = 24'h020702;
        8'd172: {VGA_R, VGA_G, VGA_B} = 24'ha6a29b;
        8'd173: {VGA_R, VGA_G, VGA_B} = 24'h74c54c;
        8'd174: {VGA_R, VGA_G, VGA_B} = 24'hf3efe5;
        8'd175: {VGA_R, VGA_G, VGA_B} = 24'h7ac520;
        8'd176: {VGA_R, VGA_G, VGA_B} = 24'h95602b;
        8'd177: {VGA_R, VGA_G, VGA_B} = 24'ha4a99e;
        8'd178: {VGA_R, VGA_G, VGA_B} = 24'h35140a;
        8'd179: {VGA_R, VGA_G, VGA_B} = 24'hcea168;
        8'd180: {VGA_R, VGA_G, VGA_B} = 24'he9ddd3;
        8'd181: {VGA_R, VGA_G, VGA_B} = 24'h9e1852;
        8'd182: {VGA_R, VGA_G, VGA_B} = 24'heae9e4;
        8'd183: {VGA_R, VGA_G, VGA_B} = 24'h6b6759;
        8'd184: {VGA_R, VGA_G, VGA_B} = 24'h661e5e;
        8'd185: {VGA_R, VGA_G, VGA_B} = 24'hf0ebe0;
        8'd186: {VGA_R, VGA_G, VGA_B} = 24'h15994a;
        8'd187: {VGA_R, VGA_G, VGA_B} = 24'h502b15;
        8'd188: {VGA_R, VGA_G, VGA_B} = 24'h9ba1a0;
        8'd189: {VGA_R, VGA_G, VGA_B} = 24'hd4ae9c;
        8'd190: {VGA_R, VGA_G, VGA_B} = 24'h8e8a7b;
        8'd191: {VGA_R, VGA_G, VGA_B} = 24'hefa2d8;
        8'd192: {VGA_R, VGA_G, VGA_B} = 24'h487175;
        8'd193: {VGA_R, VGA_G, VGA_B} = 24'hd5eba6;
        8'd194: {VGA_R, VGA_G, VGA_B} = 24'haca99f;
        8'd195: {VGA_R, VGA_G, VGA_B} = 24'h68512e;
        8'd196: {VGA_R, VGA_G, VGA_B} = 24'hae6e8a;
        8'd197: {VGA_R, VGA_G, VGA_B} = 24'hc0bfc0;
        8'd198: {VGA_R, VGA_G, VGA_B} = 24'ha59a9f;
        8'd199: {VGA_R, VGA_G, VGA_B} = 24'h693048;
        8'd200: {VGA_R, VGA_G, VGA_B} = 24'ha0e1dd;
        8'd201: {VGA_R, VGA_G, VGA_B} = 24'hd08d69;
        8'd202: {VGA_R, VGA_G, VGA_B} = 24'h61d0d3;
        8'd203: {VGA_R, VGA_G, VGA_B} = 24'h877e81;
        8'd204: {VGA_R, VGA_G, VGA_B} = 24'hdef2f0;
        8'd205: {VGA_R, VGA_G, VGA_B} = 24'hac7b67;
        8'd206: {VGA_R, VGA_G, VGA_B} = 24'h4bb7c6;
        8'd207: {VGA_R, VGA_G, VGA_B} = 24'hefba91;
        8'd208: {VGA_R, VGA_G, VGA_B} = 24'hcc1d60;
        8'd209: {VGA_R, VGA_G, VGA_B} = 24'h14994c;
        8'd210: {VGA_R, VGA_G, VGA_B} = 24'had9163;
        8'd211: {VGA_R, VGA_G, VGA_B} = 24'h53a3a6;
        8'd212: {VGA_R, VGA_G, VGA_B} = 24'hcbb262;
        8'd213: {VGA_R, VGA_G, VGA_B} = 24'h80bdc3;
        8'd214: {VGA_R, VGA_G, VGA_B} = 24'h6e6489;
        8'd215: {VGA_R, VGA_G, VGA_B} = 24'he8cf6f;
        8'd216: {VGA_R, VGA_G, VGA_B} = 24'hfcf9f6;
        8'd217: {VGA_R, VGA_G, VGA_B} = 24'hac9161;
        8'd218: {VGA_R, VGA_G, VGA_B} = 24'hd99ba5;
        8'd219: {VGA_R, VGA_G, VGA_B} = 24'h710132;
        8'd220: {VGA_R, VGA_G, VGA_B} = 24'ha59060;
        8'd221: {VGA_R, VGA_G, VGA_B} = 24'hac833d;
        8'd222: {VGA_R, VGA_G, VGA_B} = 24'h0b0805;
        8'd223: {VGA_R, VGA_G, VGA_B} = 24'ha19f98;
        8'd224: {VGA_R, VGA_G, VGA_B} = 24'h936d52;
        8'd225: {VGA_R, VGA_G, VGA_B} = 24'h4b7918;
        8'd226: {VGA_R, VGA_G, VGA_B} = 24'he1ad68;
        8'd227: {VGA_R, VGA_G, VGA_B} = 24'hafa8a3;
        8'd228: {VGA_R, VGA_G, VGA_B} = 24'h8fcc5e;
        8'd229: {VGA_R, VGA_G, VGA_B} = 24'h99670c;
        8'd230: {VGA_R, VGA_G, VGA_B} = 24'h947946;
        8'd231: {VGA_R, VGA_G, VGA_B} = 24'h6e1e2c;
        8'd232: {VGA_R, VGA_G, VGA_B} = 24'he1ae97;
        8'd233: {VGA_R, VGA_G, VGA_B} = 24'hb08c5e;
        8'd234: {VGA_R, VGA_G, VGA_B} = 24'h3c6a0f;
        8'd235: {VGA_R, VGA_G, VGA_B} = 24'hcda96c;
        8'd236: {VGA_R, VGA_G, VGA_B} = 24'hc3dbb5;
        8'd237: {VGA_R, VGA_G, VGA_B} = 24'h090406;
        8'd238: {VGA_R, VGA_G, VGA_B} = 24'h1d9ca6;
        8'd239: {VGA_R, VGA_G, VGA_B} = 24'ha4acac;
        8'd240: {VGA_R, VGA_G, VGA_B} = 24'hcf6269;
        8'd241: {VGA_R, VGA_G, VGA_B} = 24'h996126;
        8'd242: {VGA_R, VGA_G, VGA_B} = 24'hece8e3;
        8'd243: {VGA_R, VGA_G, VGA_B} = 24'h427137;
        default: {VGA_R, VGA_G, VGA_B} = 24'h000000;  // Default to black
      endcase
    end
  end

endmodule

module vga_mem (
  input logic clk,
  input logic [18:0] ra,  // 19-bit address for 640*480 pixels
  input logic [18:0] wa,                     // 19-bit address for write
  input logic write,
  input logic [7:0] wd,   // 8-bit data index
  output logic [7:0] rd   // 8-bit data index
);

  logic [7:0] data[307199:0];  // 640 * 480 = 307200 pixels
  always_ff @(posedge clk) begin
    if (write) data[wa] <= wd;
    rd <= data[ra];
  end
endmodule

module vga_counters (
    input  logic        clk50,
    reset,
    output logic [10:0] hcount,  // hcount[10:1] is pixel column
    output logic [ 9:0] vcount,  // vcount[9:0] is pixel row
    output logic        VGA_CLK,
    VGA_HS,
    VGA_VS,
    VGA_BLANK_n,
    VGA_SYNC_n
);

  /*
 * 640 X 480 VGA timing for a 50 MHz clock: one pixel every other cycle
 * 
 * HCOUNT 1599 0             1279       1599 0
 *             _______________              ________
 * ___________|    Video      |____________|  Video
 * 
 * 
 * |SYNC| BP |<-- HACTIVE -->|FP|SYNC| BP |<-- HACTIVE
 *       _______________________      _____________
 * |____|       VGA_HS          |____|
 */
  // Parameters for hcount
  parameter HACTIVE      = 11'd 1280,
             HFRONT_PORCH = 11'd 32,
             HSYNC        = 11'd 192,
             HBACK_PORCH  = 11'd 96,   
             HTOTAL       = HACTIVE + HFRONT_PORCH + HSYNC +
                            HBACK_PORCH; // 1600

  // Parameters for vcount
  parameter VACTIVE      = 10'd 480,
             VFRONT_PORCH = 10'd 10,
             VSYNC        = 10'd 2,
             VBACK_PORCH  = 10'd 33,
             VTOTAL       = VACTIVE + VFRONT_PORCH + VSYNC +
                            VBACK_PORCH; // 525

  logic endOfLine;

  always_ff @(posedge clk50 or posedge reset)
    if (reset) hcount <= 0;
    else if (endOfLine) hcount <= 0;
    else hcount <= hcount + 11'd1;

  assign endOfLine = hcount == HTOTAL - 1;

  logic endOfField;

  always_ff @(posedge clk50 or posedge reset)
    if (reset) vcount <= 0;
    else if (endOfLine)
      if (endOfField) vcount <= 0;
      else vcount <= vcount + 10'd1;

  assign endOfField = vcount == VTOTAL - 1;

  // Horizontal sync: from 0x520 to 0x5DF (0x57F)
  // 101 0010 0000 to 101 1101 1111
  assign VGA_HS = !((hcount[10:8] == 3'b101) & !(hcount[7:5] == 3'b111));
  assign VGA_VS = !(vcount[9:1] == (VACTIVE + VFRONT_PORCH) / 2);

  assign VGA_SYNC_n = 1'b0;  // For putting sync on the green signal; unused

  // Horizontal active: 0 to 1279     Vertical active: 0 to 479
  // 101 0000 0000  1280	       01 1110 0000  480
  // 110 0011 1111  1599	       10 0000 1100  524
  assign VGA_BLANK_n = !( hcount[10] & (hcount[9] | hcount[8]) ) &
			!( vcount[9] | (vcount[8:5] == 4'b1111) );

  /* VGA_CLK is 25 MHz
    *             __    __    __
    * clk50    __|  |__|  |__|
    *        
    *             _____       __
    * hcount[0]__|     |_____|
    */
  assign VGA_CLK = hcount[0];  // 25 MHz clock: rising edge sensitive

endmodule

