encapsulated model SpringMassSystem
  "Mass attached with a spring to the world frame"
  import Modelica.Icons;
  import Modelica.Mechanics.MultiBody;
  import Modelica.Mechanics.Rotational;
  import Modelica;
  extends Icons.Example;
  parameter Boolean animation=true "= true, if animation shall be enabled";
  annotation (
    experiment(StopTime=5),
    Coordsys(
      extent=[-100, -100; 100, 100],
      grid=[2, 2],
      component=[20, 20]),
    Window(
      x=0.29,
      y=0,
      width=0.71,
      height=0.85),
    Diagram,
    Documentation(info="<html>
<p>
This example shows the two different ways how force laws
can be utilized:
</p>
<ul>
<li>In the left system a body is attached via a prismatic
    joint to the world frame. The prismatic joint has two
    1-dimensional translational flanges (called \"bearing\" and \"axis\")
    that allows to connect elements from the Modelica.Mechanics.Translational
    library between the bearing and the axis connector. The effect is
    that the force generated by the 1-dimensional elements acts as driving
    force in the axis of the prismatic joint. In the example a simple
    spring is used.<br>
    The advantage of this approach is that the many elements from the
    Translational library can be easily used here and that this implementation
    is usually more efficient as when using 3-dimensional springs.</li>
<li>In the right system the same model is defined. The difference is
    that a 3-dimensional spring from the MultiBody.Forces library is used.
    This has the advantage to get a nice animation of the force component.</li>
</ul>
<p align=\"center\">
<IMG SRC=\"../Images/MultiBody/Examples/Elementary/SpringMassSystem.png\"
ALT=\"model Examples.Elementary.SpringMassSystem\">
</p>
</html>"));
  inner MultiBody.World world annotation (extent=[-80, 20; -60, 40]);
  MultiBody.Joints.ActuatedPrismatic p1(
    initType=MultiBody.Types.Init.PositionVelocity,
    n={0,-1,0},
    s_offset=0.1,
    animation=animation,
    enforceStates=true,
    boxWidth=0.05) annotation (extent=[-30, -20; -10, 0], rotation=-90);
  Modelica.Mechanics.Translational.Spring spring1(c=30)
    annotation (extent=[0, -20; 20, 0], rotation=-90);
  MultiBody.Parts.Body body1(
    m=1,
    sphereDiameter=0.2,
    animation=animation) annotation (extent=[-10, -60; -30, -40], rotation=-90);
  MultiBody.Parts.FixedTranslation bar1(animation=animation, r={0.3,0,0})
    annotation (extent=[-46, 20; -26, 40]);
  MultiBody.Parts.FixedTranslation bar2(animation=animation, r={0.3,0,0})
    annotation (extent=[0, 20; 20, 40]);
  MultiBody.Parts.Body body2(
    m=1,
    sphereDiameter=0.2,
    animation=animation) annotation (extent=[60, -60; 40, -40], rotation=-90);
  MultiBody.Joints.ActuatedPrismatic p2(
    initType=MultiBody.Types.Init.PositionVelocity,
    n={0,-1,0},
    s_offset=0.1,
    animation=animation,
    enforceStates=true,
    boxWidth=0.05) annotation (extent=[40, -20; 60, 0], rotation=-90);
  MultiBody.Forces.Spring spring2(
    c=30,
    s_unstretched=0.1,
    width=0.1) annotation (extent=[80, -20; 100, 0], rotation=-90);
equation
  connect(body1.frame_a, p1.frame_b)
    annotation (points=[-20, -39; -20, -21], style(color=0, thickness=2));
  connect(world.frame_b, bar1.frame_a)
    annotation (points=[-59, 30; -47, 30], style(color=0, thickness=2));
  connect(bar1.frame_b, p1.frame_a) annotation (points=[-25, 30; -20, 30; -20,
        1], style(color=0, thickness=2));
  connect(p1.bearing, spring1.flange_a)
    annotation (points=[-14, -6; -8, -6; -8, 0; 10, 0], style(color=58));
  connect(spring1.flange_b, p1.axis) annotation (points=[10, -20; 10, -30; -8,
        -30; -8, -18; -14, -18], style(color=58));
  connect(bar1.frame_b, bar2.frame_a)
    annotation (points=[-25, 30; -1, 30], style(color=0, thickness=2));
  connect(bar2.frame_b, p2.frame_a)
    annotation (points=[21, 30; 50, 30; 50, 1], style(color=0, thickness=2));
  connect(p2.frame_b, body2.frame_a)
    annotation (points=[50, -21; 50, -39], style(color=0, thickness=2));
  connect(bar2.frame_b, spring2.frame_a)
    annotation (points=[21, 30; 90, 30; 90, 1], style(color=0, thickness=2));
  connect(body2.frame_a, spring2.frame_b) annotation (points=[50, -39; 90, -39;
         90, -21], style(color=0, thickness=2));
end SpringMassSystem;
