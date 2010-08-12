$sixswitch_cmd="6switch"
$relay_ipv6="aaaa::11:22ff:fe33:4455"
$base_path=File.expand_path(File.dirname(__FILE__));
$base_cmd=File.join($base_path, "#{$sixswitch_cmd}")
$base_cmd << " #{$relay_ipv6}"
debug("Using $base_cmd #{$base_cmd}")

Shoes.app (:width => 480, :height => 300) do
  debug("Starting 6gui.");
  background("raven-adapter.jpg");
  stack (:margin => 4){
    #title "IPv6 Switch Control", :stroke => '#B22222'
    @log = para "Using [#{$relay_ipv6}], no action yet.", :stroke => 'black', :align => 'right'
  }
  stack (:margin => 2){
    @on = button "  on  "
    @off = button " off  "
    @status = button "status"
  }
  stack(:height => 60) {
  }
  @on.click {
    cmd="#{$base_cmd} on"
    debug("Executing #{cmd}");
    debug(system(cmd));
    @log.replace("Sent on command to [#{$relay_ipv6}].")
  }

  @off.click {
    cmd="#{$base_cmd} off"
    debug("Executing #{cmd}");
    debug(system(cmd));
    @log.replace("Sent off command to [#{$relay_ipv6}].")
  }

  @status.click {
    @log.replace("Status is not implemented yet.")
  }



end

