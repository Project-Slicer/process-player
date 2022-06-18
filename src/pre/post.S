  .text
  # Calls the post part of the Process Player
  # Parameters:
  #   - dirfd: The file descriptor of the checkpoint directory
  #   - log_fd: The file descriptor of the log file (stderr)
  #   - sp: stack pointer
  #   - entry: entry point
  .globl call_post_pp
call_post_pp:
  mv  sp, a2
  jr  a3

  .data
  .globl post_pp_begin, post_pp_end
post_pp_begin:
  .incbin "post_pp.bin"
post_pp_end:
