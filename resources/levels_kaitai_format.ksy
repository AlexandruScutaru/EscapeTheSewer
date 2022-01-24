meta:
  id: levels
  endian: le
seq:
  - id: current_level
    type: u1
  - id: num_levels
    type: u1
  - id : level_sizes
    type: u4
    repeat: expr
    repeat-expr: num_levels
  - id: levels
    type: level_type
    repeat: expr
    repeat-expr: num_levels
types:
  coords_type:
    seq:
      - id: x
        type: u2
      - id: y
        type: u2
  special_obj_type:
    seq:
      - id: count
        type: u1
      - id: coords
        type: coords_type
        repeat: expr
        repeat-expr: count
  tile_index_type:
    seq:
      - id: flip_y
        type: b1
      - id: flip_x
        type: b1
      - id: index
        type: b6
  level_type:
    seq:
      - id: width
        type: u1
      - id: height
        type: u1
      - id: tile_indices
        type: tile_index_type
        repeat: expr
        repeat-expr: width * height
      - id: start
        type: coords_type
      - id: end
        type: coords_type
      - id: slimes
        type: special_obj_type
      - id: bugs
        type: special_obj_type
      - id: health_pickups
        type: special_obj_type
  