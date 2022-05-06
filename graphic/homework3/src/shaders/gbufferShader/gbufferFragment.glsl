#ifdef GL_ES
#extension GL_EXT_draw_buffers: enable
precision highp float;
#endif

uniform sampler2D uKd;               //模型的粗糙度贴图
uniform sampler2D uNt;               //模型的法线贴图（在mesh上对vertex进行扰动的texture）
uniform sampler2D uShadowMap;        //由上一次pass渲染的光照处的深度图

varying mat4 vWorldToLight;          //光源空间基向量
varying highp vec2 vTextureCoord;
varying highp vec4 vPosWorld;        //已经归一化了
varying highp vec3 vNormalWorld;
varying highp float vDepth;

float SimpleShadowMap(vec3 posWorld,float bias){
  vec4 posLight = vWorldToLight * vec4(posWorld, 1.0);                      //p点转换至光源空间 [ -1.0,1.0 ]
  vec2 shadowCoord = clamp(posLight.xy * 0.5 + 0.5, vec2(0.0), vec2(1.0));  //p点转换至光源处摄像机屏幕上的二维坐标
  float depthSM = texture2D(uShadowMap, shadowCoord).x;                     //光源处直视p点的最浅深度
  float depth = (posLight.z * 0.5 + 0.5) * 100.0;                           //p点在光源空间深度 （*100为精度保护unpack）
  return step(0.0, depthSM - depth + bias);                                 //通过比较深度来判断p点是否应该生成阴影（非常简单的硬阴影算法）       
}

void LocalBasis(vec3 n, out vec3 b1, out vec3 b2) {
  float sign_ = sign(n.z);
  if (n.z == 0.0) {
    sign_ = 1.0;
  }
  float a = -1.0 / (sign_ + n.z);
  float b = n.x * n.y * a;
  b1 = vec3(1.0 + sign_ * n.x * n.x * a, sign_ * b, -sign_ * n.x);
  b2 = vec3(b, sign_ + n.y * n.y * a, -n.y);
}

vec3 ApplyTangentNormalMap() {
  vec3 t, b;
  LocalBasis(vNormalWorld, t, b);                               //根据法线贴图计算tbn变换基向量
  vec3 nt = texture2D(uNt, vTextureCoord).xyz * 2.0 - 1.0;      // [0-1] -> [-1,1]
  nt = normalize(nt.x * t + nt.y * b + nt.z * vNormalWorld);    //tbn变换及正则化
  return nt;
}

void main(void) {//一下0-4通道返回的贴图均为RSM贴图
  vec3 kd = texture2D(uKd, vTextureCoord).rgb;                            
  gl_FragData[0] = vec4(kd, 1.0);                                         //diffuse map
  gl_FragData[1] = vec4(vec3(vDepth), 1.0);                               //depth map
  gl_FragData[2] = vec4(ApplyTangentNormalMap(), 1.0);                    //normal map
  gl_FragData[3] = vec4(vec3(SimpleShadowMap(vPosWorld.xyz, 1e-2)), 1.0); //visibility map
  gl_FragData[4] = vec4(vec3(vPosWorld.xyz), 1.0);                        //posworld map
}