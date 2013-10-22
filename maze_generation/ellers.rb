# --------------------------------------------------------------------
# Eller's algorithm for maze generation. Novel in that it only
# requires memory proportional to the size of a single row; this means
# you can generate "bottomless" mazes with it, that just keep going
# and going and going, using not only constant memory, but little
# memory in general.
# --------------------------------------------------------------------

# --------------------------------------------------------------------
# 1. Allow the maze to be customized via command-line parameters
# --------------------------------------------------------------------

width  = (ARGV[0] || 10).to_i
height = (ARGV[1] || "-") == "-" ? nil : ARGV[1].to_i
seed   = (ARGV[2] || rand(0xFFFF_FFFF)).to_i

srand(seed)

# --------------------------------------------------------------------
# 2. Set up constants to aid with describing the passage directions
# --------------------------------------------------------------------

S, E = 1, 2

# --------------------------------------------------------------------
# 3. Data structures to assist the algorithm
# --------------------------------------------------------------------

class State
  attr_reader :width

  def initialize(width, next_set=-1)
    @width = width
    @next_set = next_set
    @sets = Hash.new { |h,k| h[k] = [] }
    @cells = {}
  end

  def next
    State.new(width, @next_set)
  end

  def populate
    width.times do |cell|
      unless @cells[cell]
        set = @next_set += 1
        @sets[set] << cell
        @cells[cell] = set
      end
    end

    self
  end

  def merge(sink_cell, target_cell)
    sink, target = @cells[sink_cell], @cells[target_cell]

    @sets[sink].concat(@sets[target])
    @sets[target].each { |cell| @cells[cell] = sink }
    @sets.delete(target)
  end

  def same?(cell1, cell2)
    @cells[cell1] == @cells[cell2]
  end

  def add(cell, set)
    @cells[cell] = set
    @sets[set] << cell
    self
  end

  def each_set
    @sets.each do |id, set|
      yield id, set
    end
  end
end

def row2str(row, last=false)
  # the \r makes sure we always start at the beginning of the line, even after
  # ctrl-C (which prints "^C" to the console)
  s = "\r|"

  row.each_with_index do |cell, index|
    south = (cell & S != 0)
    next_south = (row[index+1] && row[index+1] & S != 0)
    east = (cell & E != 0)

    s << (south ? " " : "_")

    if east
      s << ((south || next_south) ? " " : "_")
    else
      s << "|"
    end
  end

  return s
end

state = State.new(width).populate
row_count = 0

# --------------------------------------------------------------------
# 4. Eller's algorithm
# --------------------------------------------------------------------

def step(state, finish=false)
  connected_sets = []
  connected_set = [0]

  # ---
  # create the set of horizontally connected corridors in this row
  # ---
 
  (state.width-1).times do |c|
    if state.same?(c, c+1) || (!finish && rand(2) > 0)
      # cells are not joined by a passage, so we start a new connected set
      connected_sets << connected_set
      connected_set = [c+1]
    else
      state.merge(c, c+1)
      connected_set << c+1
    end
  end

  connected_sets << connected_set

  # ---
  # create the set of vertically connected corridors from this row, but
  # only if this is not the last row
  # ---
 
  verticals = []
  next_state = state.next

  unless finish
    state.each_set do |id, set|
      cells_to_connect = set.sort_by { rand }[0, 1 + rand(set.length-1)]
      verticals.concat(cells_to_connect)
      cells_to_connect.each { |cell| next_state.add(cell, id) }
    end
  end

  # ---
  # translate the connected sets and verticals into a bitmap that can be
  # returned and displayed
  # ---
 
  row = []
  connected_sets.each do |connected_set|
    connected_set.each_with_index do |cell, index|
      last = (index+1 == connected_set.length)
      map = last ? 0 : E
      map |= S if verticals.include?(cell)
      row << map
    end
  end

  [next_state.populate, row]
end

# ---
# allow ctrl-c to stop the program gracefully, letting the final row
# be generated and displayed before aborting.
# ---

spinning = true
trap("INT") { spinning = false }

puts " " + "_" * (width * 2 - 1)

while spinning
  state, row = step(state)
  row_count += 1
  puts row2str(row)
  spinning = row_count+1 < height if height
end

state, row = step(state, true)
row_count += 1

puts row2str(row)

# --------------------------------------------------------------------
# 5. Show the parameters used to build this maze, for repeatability
# --------------------------------------------------------------------

puts "#{$0} #{width} #{row_count} #{seed}"
