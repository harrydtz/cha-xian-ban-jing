
const app = getApp()
const devicesId = "653370254"
const api_key = "=pUZ5NDFVDOMsyKalWCcWNOnukI="

Page({
  data: {
    motto: 'Hello World',
    userInfo: {},
    hasUserInfo: false,
    canIUse: wx.canIUse('button.open-type.getUserInfo'),
    flag: "无人"
  },
 
  bindViewTap: function () {
    wx.navigateTo({
      url: '../logs/logs'
    })
  },
  onLoad: function () {
    if (app.globalData.userInfo) {
      this.setData({
        userInfo: app.globalData.userInfo,
        hasUserInfo: true
      })
    } else if (this.data.canIUse) {
     
      app.userInfoReadyCallback = res => {
        this.setData({
          userInfo: res.userInfo,
          hasUserInfo: true
        })
      }
    } else {
  
      wx.getUserInfo({
        success: res => {
          app.globalData.userInfo = res.userInfo
          this.setData({
            userInfo: res.userInfo,
            hasUserInfo: true
          })
        }
      })
    }

 
    const timer = setInterval(() => {
      this.getDatapoints().then(datapoints => {
        this.update(datapoints)
      })
    }, 6000)
  },
  getUserInfo: function (e) {
    console.log(e)
    app.globalData.userInfo = e.detail.userInfo
    this.setData({
      userInfo: e.detail.userInfo,
      hasUserInfo: true
    })
  },
  getDatapoints: function () {
    return new Promise((resolve, reject) => {
      wx.request({
        url: `http://api.heclouds.com/devices/653370254`,
     
        header: {
          'content-type': 'application/json',
          'api-key': api_key
        },
        success: (res) => {
          console.log(res)
          if (res.data.data.datastreams[0].datapoints[0].value == "1") {
            this.setData({
              flag: "有人"
            })

          } else {
            if (this.data.flag == "有人"){
              var that=this
              wx.showModal({
                content: "检测到用户已经离开，是否关闭电源？",
                showCancel: true,
                confirmText: '确定',
                success(res) {
                  if (res.confirm) {
                    console.log('用户点击了确定')
                    that.powerOff()
                  } else if (res.cancel) {
                    console.log('用户点击了取消')                    
                  }
                }
              })
            }
            this.setData({
              flag: "无人"
            })
          }
        },
        fail: (err) => {
          reject(err)
        }
      })
    })
  },
  update: function (datapoints) {
    const wheatherData = this.convert(datapoints);

    this.lineChart_hum.updateData({
      categories: wheatherData.categories,
      series: [{
        name: 'humidity',
        data: wheatherData.humidity,
        format: (val, name) => val.toFixed(2)
      }],
    })

    this.lineChart_light.updateData({
      categories: wheatherData.categories,
      series: [{
        name: 'light',
        data: wheatherData.light,
        format: (val, name) => val.toFixed(2)
      }],
    })

    this.lineChart_tempe.updateData({
      categories: wheatherData.categories,
      series: [{
        name: 'tempe',
        data: wheatherData.tempe,
        format: (val, name) => val.toFixed(2)
      }],
    })

  },
  powerOn: function () {
    wx.request({                                              
      url: `http://api.heclouds.com/devices/653370254`,
      data: "1",
      header: {
        'content-type': ' text/plain', 
        'api-key': api_key
      },
      method: 'POST',
      success(res) {
        console.log(res.data)
      }
    })
  },
  powerOff: function () {
    wx.request({                                                
      url: `http://api.heclouds.com/devices/653370254`, 
      data: "0",
      header: {
        'content-type': ' text/plain', 
        'api-key': api_key
      },
      method: 'POST',
      success(res) {
        console.log(res.data)
      }
    })
  }
})
