$sixswitch_cmd="6switch"
$relay_ipv6="aaaa::11:22ff:fe33:4455"
$base_path=File.expand_path(File.dirname(__FILE__));
$base_cmd=File.join($base_path, "#{$sixswitch_cmd}")
$base_cmd << " #{$relay_ipv6}"
$init_time=10*60 # 10 mins
debug("Using $base_cmd #{$base_cmd}")



Shoes.app (:width => 480, :height => 300) do
  debug("Starting 6gui.");
  background("raven-adapter.jpg");
  @time=$init_time
  @paused=true;

  def format_time(seconds)
    seconds = seconds.round
    return [seconds/3600, seconds/60 % 60, seconds % 60].map{|t| t.to_s.rjust(2,'0')}.join(':')
  end

  stack (:margin => 4){
    #title "IPv6 Switch Control", :stroke => '#B22222'
    @log = para "Using [#{$relay_ipv6}], no action yet.", :stroke => 'black', :align => 'right'
  }
  stack (:margin => 2){
    @start = button "  start  "
    @reset = button " reset  "
  }
  stack (:margin => 2){
    background whitesmoke
    @timer = title format_time(@time), :align => 'center'
  }
  stack(:height => 60) {
  }
  @start.click {
    @paused=false;
  }
  @reset.click {
    @paused=true;
    cmd="#{$base_cmd} off"
    debug("Executing #{cmd}");
    debug(system(cmd));
    @log.replace("Sent off command to [#{$relay_ipv6}].")
    @time=$init_time
  }

  animate(1) do
    @timer.replace format_time(@time) 
    @time-=1 unless @paused;
    if @time == 1 
      cmd="#{$base_cmd} on"
      debug("Executing #{cmd}");
      debug(system(cmd));
      @log.replace("Sent on command to [#{$relay_ipv6}].")
      @paused=true;
      @time=0
    end

  end
end

