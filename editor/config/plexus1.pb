text_path
{
  text : "neurotica efs"
}

noise_effector
{
  apply_to : POSITION
  displacement : {
    type : 1
    keyframes: {
      time : 0
      value: { x: 0; y: 0; z: 100}
    }
    keyframes: {
      time : 10000
      value: { x: 0; y: 0; z: 1}
    }
  }
}