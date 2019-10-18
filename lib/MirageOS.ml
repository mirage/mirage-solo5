module Main = struct
  let at_enter_iter = OS.Main.at_enter_iter
  let run = OS.Main.run
end

module Time = struct
  let sleep_ns = OS.Time.sleep_ns
end
