# --------------------------------------------------------------------
# An implementation of Wilson's algorithm for generating mazes.
# Slightly smarter than Aldous-Broder, it is novel in its use of a
# "scout" to build each path before actually recording it. Like
# Aldous-Broder, though, it is not guaranteed to ever finish, if
# the RNG makes poor choices.
#
# As with Aldous-Broder, watching the animation of its progress can
# be an exercise in frustration as you find yourself urging the cursor
# to JUST GO OVER THERE! Try it and see for yourself. :)
# --------------------------------------------------------------------
# NOTE: the display routine used in this script requires a terminal
# that supports ANSI escape sequences. Windows users, sorry. :(
# --------------------------------------------------------------------

# --------------------------------------------------------------------
# 1. Allow the maze to be customized via command-line parameters
# --------------------------------------------------------------------

width  = (ARGV[0] || 10).to_i
height = (ARGV[1] || width).to_i
seed   = (ARGV[2] || rand(0xFFFF_FFFF)).to_i

srand(seed)

grid = Array.new(height) { Array.new(width, 0) }

# --------------------------------------------------------------------
# 2. Set up constants to aid with describing the passage directions
# --------------------------------------------------------------------

N, S, E, W = 1, 2, 4, 8
IN         = 0x10
DX         = { E => 1, W => -1, N =>  0, S => 0 }
DY         = { E => 0, W =>  0, N => -1, S => 1 }
OPPOSITE   = { E => W, W =>  E, N =>  S, S => N }

# --------------------------------------------------------------------
# 3. A simple routine to emit the maze as ASCII
# --------------------------------------------------------------------

def display_maze(grid, cx=nil, cy=nil)
  print "\e[H" # move to upper-left
  puts " " + "_" * (grid[0].length * 2 - 1)
  grid.each_with_index do |row, y|
    print "|"
    row.each_with_index do |cell, x|
      print "\e[43m" if cx == x && cy == y # cursor is yellow
      if cell == 0 && y+1 < grid.length && grid[y+1][x] == 0
        print " "
      else
        print((cell & S != 0) ? " " : "_")
      end
      print "\e[0m" if cx == x && cy == y

      if cell == 0 && x+1 < row.length && row[x+1] == 0
        print((y+1 < grid.length && (grid[y+1][x] == 0 || grid[y+1][x+1] == 0)) ? " " : "_")
      elsif cell & E != 0
        print(((cell | row[x+1]) & S != 0) ? " " : "_")
      else
        print "|"
      end
    end
    puts
  end
end

# --------------------------------------------------------------------
# 4. Wilson's algorithm
# --------------------------------------------------------------------

def walk(grid)
  loop do
    cx, cy = rand(grid[0].length), rand(grid.length)
    next if grid[cy][cx] != 0

    visits = { [cx, cy] => 0 }

    start_x, start_y = cx, cy
    walking = true

    while walking
      display_maze(grid, cx, cy)
      sleep 0.02

      walking = false
      [N,S,E,W].shuffle.each do |dir|
        nx, ny = cx + DX[dir], cy + DY[dir]
        if nx >= 0 && ny >= 0 && ny < grid.length && nx < grid[ny].length
          visits[[cx, cy]] = dir

          if grid[ny][nx] != 0
            break
          else
            cx, cy = nx, ny
            walking = true
            break
          end
        end
      end
    end

    path = []
    x, y = start_x, start_y
    loop do
      dir = visits[[x, y]] or break
      path << [x, y, dir]
      x, y = x + DX[dir], y + DY[dir]
    end

    return path
  end
end

print "\e[2J" # clear screen

grid[rand(height)][rand(width)] = IN

remaining = width * height - 1

while remaining > 0
  walk(grid).each do |x, y, dir|
    nx, ny = x + DX[dir], y + DY[dir]

    grid[y][x] |= dir
    grid[ny][nx] |= OPPOSITE[dir]

    remaining -= 1

    display_maze(grid)
    sleep 0.02
  end
end

display_maze(grid)

# --------------------------------------------------------------------
# 5. Show the parameters used to build this maze, for repeatability
# --------------------------------------------------------------------

puts "#{$0} #{width} #{height} #{seed}"
